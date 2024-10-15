package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;


public class ASTFloat implements ASTExpression {
	public float f;
	private static int GLOBAL_STATE = -1;
	private static int GLOBAL_STATE_2 = -1;

	public ASTFloat(float f) {
		this.f = f;
	}

	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	public static String gen() {
		GLOBAL_STATE++;
		return ("f" + GLOBAL_STATE);
	}

	public static String gen2() {
		GLOBAL_STATE_2++;
		return "addr" + GLOBAL_STATE_2;
	}
}
