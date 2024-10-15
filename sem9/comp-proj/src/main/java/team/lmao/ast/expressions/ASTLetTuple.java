package team.lmao.ast.expressions;

import java.util.List;

import team.lmao.ast.Id;
import team.lmao.ast.types.Type;
import team.lmao.ast.visitors.Visitor;

public class ASTLetTuple implements ASTExpression {
	public final List<Id> ids;
	public final List<Type> ts;
	public final ASTExpression e1;
	public final ASTExpression e2;

	public ASTLetTuple(List<Id> ids, List<Type> ts, ASTExpression e1, ASTExpression e2) {
		this.ids = ids;
		this.ts = ts;
		this.e1 = e1;
		this.e2 = e2;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
