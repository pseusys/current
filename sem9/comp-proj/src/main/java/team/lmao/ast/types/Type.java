package team.lmao.ast.types;

import java.util.HashMap;

import team.lmao.ast.visitors.EquationVisitor;


public abstract class Type {
	private static int x = 0;
	static HashMap<String, Type> ht = new HashMap<>();

	public static Type gen() {
		Type t = new TypeVariable("?" + x++);
		ht.put(t.toString(), t);
		return t;
	}

	public abstract boolean accept(EquationVisitor v);

}
