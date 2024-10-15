package team.lmao.ast.visitors;

import java.util.ArrayList;
import java.util.HashMap;

import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;
import team.lmao.ast.types.*;

public class TypeVisitor implements Visitor<Type> {
	public HashMap<String, Type> env;

	public TypeVisitor(HashMap<String, Type> env) {

		this.env = env;

		ArrayList<Type> list = new ArrayList<>();
		list.add(new TypeInteger());
		env.put("print_int", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<Type>();
		list.add(new TypeUnit());
		env.put("exit", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<Type>();
		list.add(new TypeUnit());
		env.put("print_newline", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<Type>();
		list.add(new TypeFloat());
		env.put("truncate", new TypeFunction(list, new TypeInteger()));
	}

	public TypeVisitor() {
		this.env = new HashMap<>();

		ArrayList<Type> list = new ArrayList<>();
		list.add(new TypeInteger());
		env.put("print_int", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<>();
		list.add(new TypeUnit());
		env.put("exit", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<>();
		list.add(new TypeUnit());
		env.put("print_newline", new TypeFunction(list, new TypeUnit()));

		list = new ArrayList<>();
		list.add(new TypeFloat());
		env.put("truncate", new TypeFunction(list, new TypeInteger()));
	}

	@Override
	public Type visit(ASTUnit e) {
		return new TypeUnit();
	}

	@Override
	public Type visit(ASTBoolean e) {
		return new TypeBoolean();
	}

	@Override
	public Type visit(ASTFloat e) {
		return new TypeFloat();
	}

	@Override
	public Type visit(ASTInteger e) {
		return new TypeInteger();
	}

	@Override
	public Type visit(ASTBooleanNot e) {
		return e.e.accept(this);
	}

	@Override
	public Type visit(ASTIntegerNegative e) {
		return e.e.accept(this);
	}

	@Override
	public Type visit(ASTIntegerAdd e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTIntegerSubtract e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTFloatNegative e) {
		return e.e.accept(this);
	}

	@Override
	public Type visit(ASTFloatAdd e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTFloatSubtract e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTFloatMultiply e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTFloatDivide e) {
		Type type1 = e.e1.accept(this);
		e.e2.accept(this);
		return type1;
	}

	@Override
	public Type visit(ASTFunctionDefine e) {
		return null;
	}

	@Override
	public Type visit(ASTIntegerEqual e) {
		return new TypeBoolean();
	}

	@Override
	public Type visit(ASTIntegerLessOrEqual e) {
		return new TypeBoolean();
	}

	@Override
	public Type visit(ASTIf e) {
		e.e1.accept(this);
		e.e2.accept(this);
		Type type3 = e.e3.accept(this);

		return type3;
	}

	@Override
	public Type visit(ASTLet e) {
		if (!env.containsKey(e.id.toString())) {
			Type type = Type.gen();
			env.put(e.id.toString(), type);
		}
		e.e1.accept(this);

		return e.e2.accept(this);
	}

	@Override
	public Type visit(ASTVariable e) {
		Type type;

		if (!env.containsKey(e.id.toString())) {

			type = Type.gen();
			env.put(e.id.toString(), type);

		} else {

			type = env.get(e.id.id);
		}

		return type;
	}

	@Override
	public Type visit(ASTLetRec e) {
		Type returnType = e.fd.e.accept(this);

		while (e.fd.e.getClass().getSimpleName().equals("TypeFunction")) {
			returnType = ((TypeFunction) e.fd.e.accept(this)).returnType;
		}

		ArrayList<Type> argType = new ArrayList<>();
		for (Id i : e.fd.args) {
			if (!env.containsKey(i.id)) {

				Type t = Type.gen();
				env.put(i.id, t);
			}

			argType.add(env.get(i.id));

		}

		Type t = new TypeFunction(argType, returnType);
		env.put(e.fd.id.id, t);
		return e.e.accept(this);
	}

	@Override
	public Type visit(ASTApp e) {
		int x = 0;
		Type t = e.e.accept(this);
		ArrayList<Type> tt = new ArrayList<>();

		while (t.getClass().getSimpleName().equals("TypeFunction")) {
			t = ((TypeFunction) t).returnType;
		}

		for (ASTExpression expression : e.es) {
			Type t1 = expression.accept(this);
		}

		switch (e.e.getClass().getSimpleName()) {
			case "ASTVariable":
				for (ASTExpression expression : e.es) {
					Type t1 = expression.accept(this);
					if (t1.getClass().getSimpleName().equals("TypeFunction")) {
						t1 = ((TypeFunction) t1).returnType;
					}

					tt.add(t1);

				}

				if (!env.containsKey(((ASTVariable) e.e).id.toString()) || env.get(((ASTVariable) e.e).id.toString()).getClass().getSimpleName().equals("TypeVariable")) {

					t = Type.gen();
					TypeFunction typeFunction = new TypeFunction(tt, t);
					env.put(((ASTVariable) e.e).id.toString(), typeFunction);
				}

				break;

			case "ASTApp":
				for (ASTExpression expression : e.es) {
					Type t1 = expression.accept(this);
					if (t1.getClass().getSimpleName().equals("TypeFunction")) {
						t1 = ((TypeFunction) t1).returnType;
					}
					tt.add(t1);
				}

				if (!env.containsKey(((ASTApp) e.e).toString()) || env.get(((ASTApp) e.e).toString()).getClass().getSimpleName().equals("TypeVariable")) {

					t = Type.gen();
					TypeFunction typeFunction = new TypeFunction(tt, t);
					env.put(((ASTApp) e.e).toString(), typeFunction);

				}
				break;

			case "ASTGet":
				for (ASTExpression expression : e.es) {
					Type t1 = expression.accept(this);
					if (t1.getClass().getSimpleName().equals("TypeFunction")) {
						t1 = ((TypeFunction) t1).returnType;
					}
					tt.add(t1);
				}

				if (!env.containsKey(((ASTVariable) ((ASTGet) e.e).e1).id.toString()) || env.get(((ASTVariable) ((ASTGet) e.e).e1).id.toString()).getClass().getSimpleName().equals("TypeArray")) {
					t = Type.gen();
					TypeFunction typeFunction = new TypeFunction(tt, t);
					TypeArray ta = new TypeArray(typeFunction);
					env.put(((ASTVariable) ((ASTGet) e.e).e1).id.toString(), ta);

				}
				break;

		}

		return t;
	}

	@Override
	public Type visit(ASTTuple e) {
		ArrayList<Type> tt = new ArrayList<Type>();
		for (ASTExpression expression : e.es) {
			tt.add(expression.accept(this));

		}

		return new TypeTuple(tt);
	}

	@Override
	public Type visit(ASTLetTuple e) {
		for (Id i : e.ids) {
			if (!env.containsKey(i.toString())) {
				Type type = Type.gen();
				env.put(i.toString(), type);
			}
		}
		e.e1.accept(this);
		return e.e2.accept(this);
	}

	@Override
	public Type visit(ASTArray e) {
		Type type = e.e2.accept(this);
		return new TypeArray(type);
	}

	@Override
	public Type visit(ASTGet e) {
		Type type = e.e1.accept(this);

		if (e.e1.getClass().getSimpleName().equals("ASTVariable")) {

			type = env.get(((ASTVariable) e.e1).id.toString());
			if (!type.getClass().getSimpleName().equals("TypeArray")) {
				env.put(((ASTVariable) e.e1).id.toString(), new TypeArray(Type.gen()));
			} else {
				type = ((TypeArray) type).t;
			}
		} else if (e.e1.getClass().getSimpleName().equals("ASTArray")) {
			type = ((TypeArray) type).t;
		}

		return type;
	}

	@Override
	public Type visit(ASTPut e) {
		if (e.e1.getClass().getSimpleName().equals("ASTVariable")) {
			if (!env.containsKey(((ASTVariable) e.e1).id.toString())) {
				env.put(((ASTVariable) e.e1).id.toString(), new TypeArray(Type.gen()));
			}
			if (!env.get(((ASTVariable) e.e1).id.toString()).equals("TypeArray")) {
				env.put(((ASTVariable) e.e1).id.toString(), new TypeArray(Type.gen()));
			}

		} else {
			env.put(e.e1.toString(), new TypeArray(Type.gen()));
		}

		return new TypeUnit();
	}

	@Override
	public Type visit(ASTClosure e) {
		return null;
	}

	@Override
	public Type visit(ASTCallClosure e) {
		return null;
	}


}
