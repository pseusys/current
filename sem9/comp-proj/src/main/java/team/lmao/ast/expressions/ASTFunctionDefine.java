package team.lmao.ast.expressions;

import java.util.List;

import team.lmao.ast.Id;
import team.lmao.ast.types.Type;
import team.lmao.ast.visitors.Visitor;


public class ASTFunctionDefine implements ASTExpression {
	public Id id;
	public String label;
	public final Type type;
	public List<Id> args;
	public final ASTExpression e;
	public String free_var = "None";

	public ASTFunctionDefine(Id id, Type t, List<Id> args, ASTExpression e) {
		this.id = id;
		this.type = t;
		this.args = args;
		this.e = e;
	}


	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
