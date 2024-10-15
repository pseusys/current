package team.lmao.ast.visitors;

import team.lmao.ast.expressions.*;

public class LetReductionVisitor extends CopyVisitor {

	public ASTExpression recursiveLetRed(ASTLet e) {
		ASTLet tmpLet = new ASTLet(e.id, e.t, e.e1.accept(this), e.e2.accept(this));
		if (tmpLet.e1 instanceof ASTLet) {
			ASTLet letE1 = (ASTLet) tmpLet.e1;
			tmpLet = new ASTLet(letE1.id, letE1.t, letE1.e1, recursiveLetRed(new ASTLet(e.id, e.t, letE1.e2, tmpLet.e2)));
		}
		return tmpLet;
	}

	public ASTExpression recursiveLetTupleRed(ASTLetTuple e) {
		ASTLetTuple tmpLet = new ASTLetTuple(e.ids, e.ts, e.e1.accept(this), e.e2.accept(this));
		if (tmpLet.e1 instanceof ASTLetTuple) {
			ASTLetTuple letE1 = (ASTLetTuple) tmpLet.e1;
			tmpLet = new ASTLetTuple(letE1.ids, letE1.ts, letE1.e1, recursiveLetTupleRed(new ASTLetTuple(e.ids, e.ts, letE1.e2, tmpLet.e2)));
		}
		return tmpLet;
	}

	@Override
	public ASTExpression visit(ASTLet e) {
		return recursiveLetRed(e);
	}

	@Override
	public ASTExpression visit(ASTLetTuple e) {
		return recursiveLetTupleRed(e);
	}

	@Override
	public ASTExpression visit(ASTApp e) {
		if (e.e.accept(this) instanceof ASTLet) {
			ASTLet innerLet = (ASTLet) e.e.accept(this);
			return new ASTApp(recursiveLetRed(innerLet), e.es);
		}
		return new ASTApp(e.e.accept(this), e.es);
	}

}
