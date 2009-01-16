with TenDRA;
with TenDRA.Types;
with TenDRA.Output;
with TenDRA.Streams;

with Asis.Elements;
with Asis.Statements;
with Asis.Expressions;

with Ranges;
with Expression;
with Declaration;
with XASIS.Utils;
with XASIS.Static;
with XASIS.Classes;

package body Statement is

   use States;
   use TenDRA;
   use TenDRA.Types;

   procedure Compile
     (State    : access States.State;
      Stmt     : in     Asis.Statement;
      Loops    : in     States.Loop_Info_Access);

   function Find_Loop
     (Loops    : in     States.Loop_Info_Access;
      Stmt     : in     Asis.Statement) return Loop_Info_Access;

   Error : exception;

   -------------
   -- Compile --
   -------------

   procedure Compile
     (State    : access States.State;
      Stmt     : in     Asis.Statement;
      Loops    : in     States.Loop_Info_Access)
   is
      use Asis;
      use Asis.Statements;

      B     : TenDRA.Streams.Memory_Stream
        renames State.Units (TAGDEF).all;
      Tipe  : Asis.Element;
      Kind  : constant Statement_Kinds := Elements.Statement_Kind (Stmt);
   begin
      case Kind is
         when A_Null_Statement =>
            Output.TDF (B, c_make_top);

         when An_Assignment_Statement =>
            Tipe := Expressions.Corresponding_Expression_Type
              (Assignment_Variable_Name (Stmt));

            Output.TDF (B, c_assign);

            Expression.Target_Name
              (State, Assignment_Variable_Name (Stmt), B, TAGDEF);

            Expression.Compile
              (State,
               Assignment_Expression (Stmt),
               XASIS.Classes.Type_From_Declaration (Tipe),
               False, B, TAGDEF);

         when An_If_Statement =>
            declare
               List     : Path_List := Statement_Paths (Stmt);
               Has_Else : Boolean := False;
               Label    : Small;
            begin
               for J in List'Range loop
                  case Elements.Path_Kind (List (J)) is
                     when An_If_Path | An_Elsif_Path
                       =>
                        Label := State.Labels (TAGDEF);
                        Inc (State.Labels (TAGDEF));
                        Output.TDF (B, c_conditional);
                        Output.TDF (B, c_make_label);
                        Output.TDFINT (B, Label);
                        Output.TDF (B, c_sequence);
                        Output.List_Count (B, 1);

                        Expression.Compile_Boolean
                          (State,
                           Condition_Expression (List (J)),
                           False,
                           Label, B, TAGDEF);

                        Compile
                          (State, Sequence_Of_Statements (List (J)), Loops);
                     when An_Else_Path =>
                        Compile
                          (State, Sequence_Of_Statements (List (J)), Loops);
                        Has_Else := True;
                     when others =>
                        raise Error;
                  end case;
               end loop;

               if not Has_Else then
                  Output.TDF (B, c_make_top);
               end if;
            end;
--        A_Case_Statement
         when A_Loop_Statement
           | A_While_Loop_Statement
           | A_For_Loop_Statement
           =>
            declare
               Info     : aliased Loop_Info;
               For_Inc  : Boolean;
               For_Tag  : Small;
               For_Def  : Asis.Discrete_Subtype_Definition;
               For_Decl : Asis.Declaration;
               For_Type : XASIS.Classes.Type_Info;
               Context  : Ranges.Range_Context;
            begin
               Info.Next         := Loops;
               Info.Element      := Stmt;
               Info.Repeat_Label :=State.Labels (TAGDEF);
               Inc (State.Labels (TAGDEF));
               Info.Exit_Label   :=State.Labels (TAGDEF);
               Inc (State.Labels (TAGDEF));

               if Kind = A_For_Loop_Statement then
                  For_Decl := For_Loop_Parameter_Specification (Stmt);
                  For_Type := XASIS.Classes.Type_Of_Declaration (For_Decl);

                  Declaration.Loop_Parameter
                    (State, For_Decl, For_Def, For_Tag, For_Inc, Context);
               end if;

               Output.TDF (B, c_repeat);
               Output.TDF (B, c_make_label);
               Output.TDFINT (B, Info.Repeat_Label);
               Output.TDF (B, c_make_top);

               Output.TDF (B, c_conditional);
               Output.TDF (B, c_make_label);
               Output.TDFINT (B, Info.Exit_Label);

               Output.TDF (B, c_sequence);

               if Kind = A_For_Loop_Statement then
                  Output.List_Count (B, 3);
                  Output.TDF (B, c_integer_test);
                  Output.No_Option (B);  --  prob

                  if For_Inc then
                     Output.TDF (B, c_less_than_or_equal);
                  else
                     Output.TDF (B, c_greater_than_or_equal);
                  end if;

                  Output.TDF (B, c_make_label);
                  Output.TDFINT (B, Info.Exit_Label);

                  Output.TDF (B, c_contents);
                  Declaration.Output_Shape (State, For_Type, B, TAGDEF);

                  Output.TDF (B, c_obtain_tag);
                  Output.TDF (B, c_make_tag);
                  Output.TDFINT (B, For_Tag);

                  if For_Inc then
                     Ranges.Compile_Discrete
                       (State, For_Def, For_Type, False, B, TAGDEF, Context,
                        XASIS.Static.Upper);
                  else
                     Ranges.Compile_Discrete
                       (State, For_Def, For_Type, False, B, TAGDEF, Context,
                        XASIS.Static.Lower);
                  end if;

               elsif Kind = A_While_Loop_Statement then
                  Output.List_Count (B, 2);
                  Expression.Compile_Boolean
                    (State,
                     While_Condition (Stmt),
                     False,
                     Info.Exit_Label,
                     B, TAGDEF);
               else
                  Output.List_Count (B, 1);
               end if;

               Compile (State, Loop_Statements (Stmt), Info'Unchecked_Access);

               if Kind = A_For_Loop_Statement then
                  declare
                     Decl  : Asis.Declaration :=
                       XASIS.Classes.Get_Declaration (For_Type);
                     Arg   : Streams.Memory_Stream;
                     Kind  : Asis.Attribute_Kinds;
                     Next  : Small;
                  begin
                     if For_Inc then
                        Kind := A_Succ_Attribute;
                     else
                        Kind := A_Pred_Attribute;
                     end if;

                     Next := Find_Attribute (State, Decl, Kind, TAGDEF);

                     Output.TDF (B, c_assign);
                     Output.TDF (B, c_obtain_tag);
                     Output.TDF (B, c_make_tag);
                     Output.TDFINT (B, For_Tag);

                     Output.TDF (B, c_exp_apply_token);
                     Output.TDF (B, c_make_tok);
                     Output.TDFINT (B, Next);

                     Streams.Expect
                       (Arg, Dummy, (1 => (EXP_SORT, Singular, False)));

                     Output.TDF (Arg, c_contents);
                     Declaration.Output_Shape (State, For_Type, Arg, TAGDEF);

                     Output.TDF (Arg, c_obtain_tag);
                     Output.TDF (Arg, c_make_tag);
                     Output.TDFINT (Arg, For_Tag);

                     Output.BITSTREAM (B, Arg);
                  end;
               end if;

               Output.TDF (B, c_goto);
               Output.TDF (B, c_make_label);
               Output.TDFINT (B, Info.Repeat_Label);

               Output.TDF (B, c_make_top);
            end;

         when A_Block_Statement =>
            Compile (State, Block_Statements (Stmt), Loops);

         when An_Exit_Statement =>
            declare
               Info : Loop_Info_Access := Find_Loop (Loops, Stmt);
            begin
               if Elements.Is_Nil (Exit_Condition (Stmt)) then
                  Output.TDF (B, c_goto);
                  Output.TDF (B, c_make_label);
                  Output.TDFINT (B, Info.Exit_Label);
               else
                  Expression.Compile_Boolean
                    (State,
                     Exit_Condition (Stmt),
                     True,
                     Info.Exit_Label,
                     B, TAGDEF);
               end if;
            end;
--        A_Goto_Statement
         when A_Procedure_Call_Statement =>
            Expression.Function_Call
              (State, Stmt, XASIS.Classes.Not_A_Type, False, B, TAGDEF);

         when A_Return_Statement =>
            declare
               Decl   : Asis.Declaration :=
                 XASIS.Utils.Parent_Declaration (Stmt);
               Result : Asis.Expression :=
                 XASIS.Utils.Get_Result_Profile (Decl);
               Tipe   : XASIS.Classes.Type_Info;
            begin
               Output.TDF (B, c_return);

               if Asis.Elements.Is_Nil (Result) then
                  Output.TDF (B, c_make_top);
               else
                  Tipe := XASIS.Classes.Type_From_Subtype_Mark (Result);
                  Expression.Compile
                    (State, Return_Expression (Stmt), Tipe, False, B, TAGDEF);
               end if;
            end;

--        An_Accept_Statement
--        An_Entry_Call_Statement
--        A_Requeue_Statement
--        A_Requeue_Statement_With_Abort
--        A_Delay_Until_Statement
--        A_Delay_Relative_Statement
--        A_Terminate_Alternative_Statement
--        A_Selective_Accept_Statement
--        A_Timed_Entry_Call_Statement
--        A_Conditional_Entry_Call_Statement
--        An_Asynchronous_Select_Statement
--        An_Abort_Statement
--        A_Raise_Statement
--        A_Code_Statement
         when others =>
            raise Error;
      end case;
   end Compile;

   -------------
   -- Compile --
   -------------

   procedure Compile
     (State    : access States.State;
      List     : in     Asis.Statement_List;
      Loops    : in     States.Loop_Info_Access)
   is
      B     : TenDRA.Streams.Memory_Stream
        renames State.Units (TAGDEF).all;
   begin
      if List'Length = 0 then
         Output.TDF (B, c_make_top);
         return;
      elsif List'Length = 1 then
         Compile (State, List (List'First), Loops);
         return;
      end if;

      Output.TDF (B, c_sequence);
      Output.List_Count (B, List'Length - 1);

      for J in List'Range loop
         Compile (State, List (J), Loops);
      end loop;
   end Compile;

   ---------------
   -- Find_Loop --
   ---------------

   function Find_Loop
     (Loops    : in     States.Loop_Info_Access;
      Stmt     : in     Asis.Statement) return Loop_Info_Access
   is
      use Asis.Elements;
      use Asis.Statements;
      Ptr    : Loop_Info_Access := Loops;
      Target : Asis.Statement := Corresponding_Loop_Exited (Stmt);
   begin
      while Ptr /= null loop
         if Is_Equal (Ptr.Element, Target) then
            return Ptr;
         end if;

         Ptr := Ptr.Next;
      end loop;

      raise Error;
   end Find_Loop;

end Statement;


------------------------------------------------------------------------------
--  Copyright (c) 2006, Maxim Reznik
--  All rights reserved.
--
--  Redistribution and use in source and binary forms, with or without
--  modification, are permitted provided that the following conditions are met:
--
--     * Redistributions of source code must retain the above copyright notice,
--     * this list of conditions and the following disclaimer.
--     * Redistributions in binary form must reproduce the above copyright
--     * notice, this list of conditions and the following disclaimer in the
--     * documentation and/or other materials provided with the distribution.
--
--  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
--  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
--  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
--  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
--  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
--  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
--  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
--  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
--  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
--  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
--  POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------