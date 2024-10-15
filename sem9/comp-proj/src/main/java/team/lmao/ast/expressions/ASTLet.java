package team.lmao.ast.expressions;

import team.lmao.ast.Id;
import team.lmao.ast.types.Type;
import team.lmao.ast.visitors.Visitor;

public class ASTLet implements ASTExpression {
	public Id id;
	public final Type t;
	public final ASTExpression e1;
	public final ASTExpression e2;

	public ASTLet(Id id, Type t, ASTExpression e1, ASTExpression e2) {
		this.id = id;
		this.t = t;
		this.e1 = e1;
		this.e2 = e2;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
