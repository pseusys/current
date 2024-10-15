package team.lmao.ast.visitors;

import team.lmao.ast.expressions.*;

public interface Visitor<E> {
	E visit(ASTUnit e);

	E visit(ASTBoolean e);

	E visit(ASTInteger e);

	E visit(ASTFloat e);

	E visit(ASTBooleanNot e);

	E visit(ASTIntegerNegative e);

	E visit(ASTIntegerAdd e);

	E visit(ASTIntegerSubtract e);

	E visit(ASTFloatNegative e);

	E visit(ASTFloatAdd e);

	E visit(ASTFloatSubtract e);

	E visit(ASTFloatMultiply e);

	E visit(ASTFloatDivide e);

	E visit(ASTFunctionDefine e);

	E visit(ASTIntegerEqual e);

	E visit(ASTIntegerLessOrEqual e);

	E visit(ASTIf e);

	E visit(ASTLet e);

	E visit(ASTVariable e);

	E visit(ASTLetRec e);

	E visit(ASTApp e);

	E visit(ASTTuple e);

	E visit(ASTLetTuple e);

	E visit(ASTArray e);

	E visit(ASTGet e);

	E visit(ASTPut e);

	E visit(ASTClosure e);

	E visit(ASTCallClosure e);
}
