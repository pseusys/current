package team.lmao.ast.expressions;

import java.util.List;

import team.lmao.ast.Id;
import team.lmao.ast.visitors.Visitor;

public class ASTCallClosure implements ASTExpression {
	public Id id;
	public List<Id> args;

	public ASTCallClosure(Id id, List<Id> args) {
		this.id = id;
		this.args = args;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

}
