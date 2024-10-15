package team.lmao.ast.visitors;

import team.lmao.ast.expressions.*;


public class CopyVisitor implements Visitor<ASTExpression> {
	@Override
	public ASTExpression visit(ASTUnit e) {
		return new ASTUnit();
	}

	@Override
	public ASTExpression visit(ASTBoolean e) {
		return new ASTBoolean(e.b);
	}

	@Override
	public ASTExpression visit(ASTInteger e) {
		return new ASTInteger(e.i);
	}

	@Override
	public ASTExpression visit(ASTFloat e) {
		return new ASTFloat(e.f);
	}

	@Override
	public ASTExpression visit(ASTBooleanNot e) {
		return new ASTBooleanNot(e.e.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIntegerNegative e) {
		return new ASTIntegerNegative(e.e.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIntegerAdd e) {
		return new ASTIntegerAdd(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIntegerSubtract e) {
		return new ASTIntegerSubtract(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTFloatNegative e) {
		return new ASTFloatNegative(e.e.accept(this));
	}

	@Override
	public ASTExpression visit(ASTFloatAdd e) {
		return new ASTFloatAdd(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTFloatSubtract e) {
		return new ASTFloatSubtract(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTFloatMultiply e) {
		return new ASTFloatMultiply(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTFloatDivide e) {
		return new ASTFloatDivide(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIntegerEqual e) {
		return new ASTIntegerEqual(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIntegerLessOrEqual e) {
		return new ASTIntegerLessOrEqual(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTIf e) {
		return new ASTIf(e.e1.accept(this), e.e2.accept(this), e.e3.accept(this));
	}

	@Override
	public ASTExpression visit(ASTLet e) {
		return new ASTLet(e.id, e.t, e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTVariable e) {
		return new ASTVariable(e.id);
	}

	@Override
	public ASTExpression visit(ASTLetRec e) {
		ASTFunctionDefine knormalizedFd = new ASTFunctionDefine(e.fd.id, e.fd.type, e.fd.args, e.fd.e.accept(this));
		return new ASTLetRec(knormalizedFd, e.e.accept(this));
	}


	@Override
	public ASTExpression visit(ASTFunctionDefine e) {
		return new ASTFunctionDefine(e.id, e.type, e.args, e.e.accept(this));
	}

	@Override
	public ASTExpression visit(ASTApp e) {
		return new ASTApp(e.e.accept(this), e.es);
	}

	@Override
	public ASTExpression visit(ASTTuple e) {
		return new ASTTuple(e.es);
	}

	@Override
	public ASTExpression visit(ASTLetTuple e) {
		return new ASTLetTuple(e.ids, e.ts, e.e1, e.e2);
	}

	@Override
	public ASTExpression visit(ASTArray e) {
		return new ASTArray(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTGet e) {
		return new ASTGet(e.e1.accept(this), e.e2.accept(this));
	}

	@Override
	public ASTExpression visit(ASTPut e) {
		return new ASTPut(e.e1.accept(this), e.e2.accept(this), e.e3.accept(this));
	}

	@Override
	public ASTExpression visit(ASTClosure e) {
		return e;
	}

	@Override
	public ASTExpression visit(ASTCallClosure e) {
		return e;
	}

}
