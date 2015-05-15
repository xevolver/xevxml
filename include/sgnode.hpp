/**
 * @file     sgnode.hpp
 * @brief    list of ROSE Sage III AST classes supported by XevXml
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2013 Hiroyuki TAKIZAWA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* --- INTERNAL USE ONLY --- */

/* This header file might be read several times */
#ifdef SAGE3
#define SAGE3_BOP(x) SAGE3(x)
#define SAGE3_UOP(x) SAGE3(x)
#define SAGE3_VAL(x) SAGE3(x)
#define SAGE3_EXPR(x) SAGE3(x)
#define SAGE3_LSUP(x) SAGE3(x)
#define SAGE3_DECL(x) SAGE3(x)
#define SAGE3_STMT(x) SAGE3(x)
#define SAGE3_SUPP(x) SAGE3(x)
#define SAGE3_TYPE(x) SAGE3(x)
#else
#define SAGE3
#ifndef SAGE3_BOP
#define SAGE3_BOP(x)
#endif
#ifndef SAGE3_UOP
#define SAGE3_UOP(x)
#endif
#ifndef SAGE3_VAL
#define SAGE3_VAL(x)
#endif
#ifndef SAGE3_EXPR
#define SAGE3_EXPR(x)
#endif
#ifndef SAGE3_LSUP
#define SAGE3_LSUP(x)
#endif
#ifndef SAGE3_DECL
#define SAGE3_DECL(x)
#endif
#ifndef SAGE3_STMT
#define SAGE3_STMT(x)
#endif
#ifndef SAGE3_SUPP
#define SAGE3_SUPP(x)
#endif
#ifndef SAGE3_TYPE
#define SAGE3_TYPE(x)
#endif
#endif

// -------------------------------------
SAGE3_BOP(AddOp);
SAGE3_BOP(AndAssignOp);
SAGE3_BOP(AndOp);
SAGE3_BOP(ArrowExp);
SAGE3_BOP(AssignOp);
SAGE3_BOP(BitAndOp);
SAGE3_BOP(BitOrOp);
SAGE3_BOP(BitXorOp);
SAGE3_BOP(CommaOpExp);
SAGE3_BOP(ConcatenationOp);
SAGE3_BOP(DivAssignOp);
SAGE3_BOP(DivideOp);
SAGE3_BOP(EqualityOp);
SAGE3_BOP(ExponentiationOp);
SAGE3_BOP(GreaterOrEqualOp);
SAGE3_BOP(GreaterThanOp);
SAGE3_BOP(IorAssignOp);
SAGE3_BOP(LessOrEqualOp);
SAGE3_BOP(LessThanOp);
SAGE3_BOP(LshiftAssignOp);
SAGE3_BOP(LshiftOp);
SAGE3_BOP(MinusAssignOp);
SAGE3_BOP(ModAssignOp);
SAGE3_BOP(ModOp);
SAGE3_BOP(MultAssignOp);
SAGE3_BOP(MultiplyOp);
SAGE3_BOP(NotEqualOp);
SAGE3_BOP(OrOp);
SAGE3_BOP(PlusAssignOp);
SAGE3_BOP(PointerAssignOp);
SAGE3_BOP(PntrArrRefExp);

SAGE3_BOP(RshiftAssignOp);
SAGE3_BOP(RshiftOp);
SAGE3_BOP(SubtractOp);
SAGE3_BOP(UserDefinedBinaryOp);
SAGE3_BOP(UserDefinedUnaryOp);
SAGE3_BOP(XorAssignOp);

SAGE3_UOP(AddressOfOp);
SAGE3_UOP(BitComplementOp);
SAGE3_UOP(MinusMinusOp);
SAGE3_UOP(MinusOp);
SAGE3_UOP(NotOp);
SAGE3_UOP(PlusPlusOp);
SAGE3_UOP(UnaryAddOp);

SAGE3_VAL(BoolValExp);
SAGE3_VAL(CharVal);
SAGE3_VAL(ComplexVal);
SAGE3_VAL(DoubleVal);
SAGE3_VAL(EnumVal);
SAGE3_VAL(FloatVal);
SAGE3_VAL(IntVal);
SAGE3_VAL(LongDoubleVal);
SAGE3_VAL(LongIntVal);
SAGE3_VAL(LongLongIntVal);
SAGE3_VAL(ShortVal);
SAGE3_VAL(StringVal);
SAGE3_VAL(UnsignedCharVal);
SAGE3_VAL(UnsignedIntVal);
SAGE3_VAL(UnsignedLongLongIntVal);
SAGE3_VAL(UnsignedLongVal);
SAGE3_VAL(UnsignedShortVal);
SAGE3_VAL(WcharVal);

// -------------------------------------
SAGE3_EXPR(ActualArgumentExpression);
SAGE3_EXPR(AggregateInitializer);
SAGE3_EXPR(AssignInitializer);
SAGE3_EXPR(AsteriskShapeExp);
SAGE3_EXPR(CastExp);
SAGE3_EXPR(ColonShapeExp);
//SAGE3_EXPR(CompoundInitializer);
SAGE3_EXPR(ConditionalExp);
SAGE3_EXPR(ConstructorInitializer);
SAGE3_EXPR(DotExp);
SAGE3_EXPR(ExprListExp);
SAGE3_EXPR(FunctionCallExp);
SAGE3_EXPR(FunctionRefExp);
SAGE3_EXPR(ImpliedDo);
SAGE3_EXPR(LabelRefExp);
SAGE3_EXPR(NullExpression);
//SAGE3_EXPR(PntrArrRefExp);
SAGE3_EXPR(PointerDerefExp);
SAGE3_EXPR(SizeOfOp);
SAGE3_EXPR(StatementExpression);
SAGE3_EXPR(SubscriptExpression);
SAGE3_EXPR(VarArgEndOp);
SAGE3_EXPR(VarArgOp);
SAGE3_EXPR(VarArgStartOp);
SAGE3_EXPR(VarRefExp);
// -------------------------------------
SAGE3_LSUP(CommonBlockObject);
SAGE3_LSUP(InitializedName);
SAGE3_LSUP(InterfaceBody);
SAGE3_LSUP(RenamePair);
// -------------------------------------
SAGE3_DECL(AsmStmt);
SAGE3_DECL(AttributeSpecificationStatement);
SAGE3_DECL(ClassDeclaration);
SAGE3_DECL(CommonBlock);
SAGE3_DECL(ContainsStatement);
SAGE3_DECL(DerivedTypeStatement);
SAGE3_DECL(EntryStatement);
SAGE3_DECL(EnumDeclaration);
SAGE3_DECL(EquivalenceStatement);
SAGE3_DECL(FormatStatement);
SAGE3_DECL(FortranIncludeLine);
SAGE3_DECL(FunctionDeclaration);
SAGE3_DECL(FunctionParameterList);
SAGE3_DECL(ImplicitStatement);
SAGE3_DECL(InterfaceStatement);
SAGE3_DECL(ModuleStatement);
SAGE3_DECL(NamelistStatement);
SAGE3_DECL(PragmaDeclaration);
SAGE3_DECL(ProcedureHeaderStatement);
SAGE3_DECL(ProgramHeaderStatement);
SAGE3_DECL(TypedefDeclaration);
SAGE3_DECL(UseStatement);
SAGE3_DECL(VariableDeclaration);
// -------------------------------------
SAGE3_STMT(AllocateStatement);
SAGE3_STMT(ArithmeticIfStatement);
SAGE3_STMT(BackspaceStatement);
SAGE3_STMT(BasicBlock);
SAGE3_STMT(BreakStmt);
SAGE3_STMT(CaseOptionStmt);
SAGE3_STMT(ClassDefinition);
SAGE3_STMT(CloseStatement);
SAGE3_STMT(ComputedGotoStatement);
SAGE3_STMT(ContinueStmt);
SAGE3_STMT(DeallocateStatement);
SAGE3_STMT(DefaultOptionStmt);
SAGE3_STMT(DoWhileStmt);
SAGE3_STMT(ElseWhereStatement);
SAGE3_STMT(EndfileStatement);
SAGE3_STMT(ExprStatement);
SAGE3_STMT(FlushStatement);
SAGE3_STMT(ForAllStatement);
SAGE3_STMT(ForInitStatement);
SAGE3_STMT(ForStatement);
SAGE3_STMT(FortranDo);
SAGE3_STMT(FunctionDefinition);
SAGE3_STMT(Global);
SAGE3_STMT(GotoStatement);
SAGE3_STMT(IfStmt);
SAGE3_STMT(InquireStatement);
SAGE3_STMT(LabelStatement);
SAGE3_STMT(NullStatement);
SAGE3_STMT(NullifyStatement);
SAGE3_STMT(OpenStatement);
SAGE3_STMT(PrintStatement);
SAGE3_STMT(ReadStatement);
SAGE3_STMT(ReturnStmt);
SAGE3_STMT(RewindStatement);
SAGE3_STMT(StopOrPauseStatement);
SAGE3_STMT(SwitchStatement);
SAGE3_STMT(WaitStatement);
SAGE3_STMT(WhereStatement);
SAGE3_STMT(WhileStmt);
SAGE3_STMT(WriteStatement);
// -------------------------------------
SAGE3_SUPP(DataStatementGroup);
SAGE3_SUPP(DataStatementObject);
SAGE3_SUPP(DataStatementValue);
SAGE3_SUPP(FormatItem);
SAGE3_SUPP(FunctionParameterTypeList);
SAGE3_SUPP(NameGroup);
SAGE3_SUPP(Pragma);
SAGE3_SUPP(SourceFile);
SAGE3_SUPP(TypedefSeq);
// -------------------------------------
SAGE3_TYPE(ArrayType);
SAGE3_TYPE(ClassType);
SAGE3_TYPE(EnumType);
SAGE3_TYPE(FunctionType);
SAGE3_TYPE(ModifierType);
SAGE3_TYPE(PointerType);
SAGE3_TYPE(TypeBool);
SAGE3_TYPE(TypeChar);
SAGE3_TYPE(TypeComplex);
SAGE3_TYPE(TypeCrayPointer);
SAGE3_TYPE(TypeDefault);
SAGE3_TYPE(TypeDouble);
SAGE3_TYPE(TypeEllipse);
SAGE3_TYPE(TypeFloat);
SAGE3_TYPE(TypeImaginary);
SAGE3_TYPE(TypeInt);
SAGE3_TYPE(TypeLabel);
SAGE3_TYPE(TypeLong);
SAGE3_TYPE(TypeLongDouble);
SAGE3_TYPE(TypeLongLong);
SAGE3_TYPE(TypeShort);
SAGE3_TYPE(TypeSignedChar);
SAGE3_TYPE(TypeSignedInt);
SAGE3_TYPE(TypeSignedLong);
SAGE3_TYPE(TypeSignedLongLong);
SAGE3_TYPE(TypeSignedShort);
SAGE3_TYPE(TypeString);
SAGE3_TYPE(TypeUnsignedChar);
SAGE3_TYPE(TypeUnsignedInt);
SAGE3_TYPE(TypeUnsignedLong);
SAGE3_TYPE(TypeUnsignedLongLong);
SAGE3_TYPE(TypeUnsignedShort);
SAGE3_TYPE(TypeVoid);
SAGE3_TYPE(TypedefType);

#undef SAGE3
#undef SAGE3_BOP
#undef SAGE3_UOP
#undef SAGE3_VAL
#undef SAGE3_EXPR
#undef SAGE3_LSUP
#undef SAGE3_DECL
#undef SAGE3_STMT
#undef SAGE3_SUPP
#undef SAGE3_TYPE
