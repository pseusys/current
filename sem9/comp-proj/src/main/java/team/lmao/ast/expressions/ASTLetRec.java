package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public class ASTLetRec implements ASTExpression {
	public ASTFunctionDefine fd;
	public final ASTExpression e;

	public ASTLetRec(ASTFunctionDefine fd, ASTExpression e) {
		this.fd = fd;
		this.e = e;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
