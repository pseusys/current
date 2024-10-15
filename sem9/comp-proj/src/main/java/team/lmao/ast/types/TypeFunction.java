package team.lmao.ast.types;

import java.util.ArrayList;

import team.lmao.ast.visitors.EquationVisitor;


public class TypeFunction extends Type {
	public ArrayList<Type> argType;
	public Type returnType;

	public TypeFunction(ArrayList<Type> argType, Type returnType) {
		this.argType = argType;
		this.returnType = returnType;
	}

	@Override
	public String toString() {
		String ret = new String();

		for (int i = 0; i < argType.size(); i++) {
			if (argType.get(i).getClass().getSimpleName().equals("TFun")) {
				ret = ret.concat("(");
			}

			ret = ret.concat(argType.get(i).toString());

			if (argType.get(i).getClass().getSimpleName().equals("TFun")) {
				ret = ret.concat(")");
			}

			if (i < argType.size()) {
				ret = ret.concat("=>");
			}
		}

		if (returnType.getClass().getSimpleName().equals("TFun")) {
			ret = ret.concat("(");
		}

		ret = ret.concat(returnType.toString());

		if (returnType.getClass().getSimpleName().equals("TFun")) {
			ret = ret.concat(")");
		}
		return ret;
	}

	@Override
	public boolean accept(EquationVisitor v) {
		return v.visit(this);
	}
}
