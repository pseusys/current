package team.lmao.ast.typechecking;

import java.util.ArrayList;
import java.util.HashMap;

import team.lmao.ast.expressions.*;
import team.lmao.ast.types.Type;
import team.lmao.ast.types.TypeArray;
import team.lmao.ast.types.TypeBoolean;
import team.lmao.ast.types.TypeFloat;
import team.lmao.ast.types.TypeFunction;
import team.lmao.ast.types.TypeInteger;
import team.lmao.ast.types.TypeTuple;
import team.lmao.ast.types.TypeUnit;
import team.lmao.ast.visitors.EquationVisitor;
import team.lmao.ast.visitors.TypeVisitor;
import team.lmao.exceptions.NotImplementedException;

public class EquationGen {
	private static final ArrayList<Equation> listEq = new ArrayList<>(); // List of equations
	private static final ArrayList<String> varSeen = new ArrayList<>();

	/**
	 * @param e : the root of the tree, the point where we start.
	 */
	public static void start(ASTExpression e) {
		TypeVisitor tv = new TypeVisitor();
		e.accept(tv);
		ArrayList<Equation> listtEq = GenEquations(tv.env, e, new TypeUnit());
		SolveEquations(listtEq, tv.env);
	}

	private static ArrayList<Equation> GenEquations(HashMap<String, Type> env, ASTExpression e, Type t) {
		TypeVisitor tv = new TypeVisitor(env);
		Type t1;
		HashMap<String, Type> envFun;
		String newkey;

		boolean newDic = false;
		if (e instanceof ASTUnit) {
			listEq.add(new Equation(new TypeUnit(), t));
		} else if (e instanceof ASTInteger) {
			listEq.add(new Equation(new TypeInteger(), t));
		} else if (e instanceof ASTFloat) {
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTBoolean) {
			listEq.add(new Equation(new TypeBoolean(), t));
		} else if (e instanceof ASTBooleanNot) {
			GenEquations(env, ((ASTBooleanNot) e).e, new TypeBoolean());
			listEq.add(new Equation(new TypeBoolean(), t));
		} else if (e instanceof ASTIntegerNegative) {
			GenEquations(env, ((ASTIntegerNegative) e).e, new TypeInteger());
			listEq.add(new Equation(new TypeInteger(), t));
		} else if (e instanceof ASTIntegerAdd) {
			GenEquations(env, ((ASTIntegerAdd) e).e1, new TypeInteger());
			GenEquations(env, ((ASTIntegerAdd) e).e2, new TypeInteger());
			listEq.add(new Equation(new TypeInteger(), t));
		} else if (e instanceof ASTIntegerSubtract) {
			GenEquations(env, ((ASTIntegerSubtract) e).e1, new TypeInteger());
			GenEquations(env, ((ASTIntegerSubtract) e).e2, new TypeInteger());
			listEq.add(new Equation(new TypeInteger(), t));
		} else if (e instanceof ASTLet) {
			ASTLet l = (ASTLet) e;
			String id = l.id.toString();
			envFun = new HashMap<>();
			envFun.putAll(env);
			if (!varSeen.contains(l.id.toString()) && !env.get(l.id.toString()).getClass().getSimpleName().equals("TypeFunction")) {
				varSeen.add(l.id.toString());
				t1 = env.get(l.id.toString());
			} else {
				newDic = true;
				t1 = Type.gen();
				envFun.put((id), t1);
			}
			GenEquations(env, l.e1, t1);

			Type prov = env.get(l.id.toString());
			env.put(l.id.toString(), t1);

			GenEquations(envFun, l.e2, t);

			env.put(l.id.toString(), prov);

			if (newDic) {
				newkey = id;
				while (env.containsKey(newkey)) {
					newkey = newkey.concat("'");
				}
				env.put(newkey, (Type) envFun.get(id));
			}
		} else if (e instanceof ASTVariable) {
			t1 = e.accept(tv);
			if (!t1.getClass().getSimpleName().equals("TypeFunction")) {
				listEq.add(new Equation(t1, t));
			}
		} else if (e instanceof ASTIf) {
			ASTIf iff = (ASTIf) e;
			GenEquations(env, iff.e1, new TypeBoolean());
			GenEquations(env, iff.e2, t);
			GenEquations(env, iff.e3, t);
		} else if (e instanceof ASTIntegerEqual) {
			GenEquations(env, ((ASTIntegerEqual) e).e1, new TypeInteger());
			GenEquations(env, ((ASTIntegerEqual) e).e2, new TypeInteger());
		} else if (e instanceof ASTIntegerLessOrEqual) {
			GenEquations(env, ((ASTIntegerLessOrEqual) e).e1, new TypeInteger());
			GenEquations(env, ((ASTIntegerLessOrEqual) e).e2, new TypeInteger());
		} else if (e instanceof ASTLetRec) {
			ASTLetRec letrec = (ASTLetRec) e;
			String idletRec = letrec.fd.id.toString();

			// We create an identical env in which to check types for the body of the function
			envFun = new HashMap<String, Type>();
			envFun.putAll(env);

			// We check if we've already seen this before
			if (!varSeen.contains(idletRec)) {
				varSeen.add(idletRec);
			} else {
				envFun.put((idletRec), letrec.accept(tv));
				newDic = true;
			}
			GenEquations(env, letrec.e, t);

			GenEquations(envFun, letrec.fd.e, ((TypeFunction) env.get(idletRec)).returnType);
		} else if (e instanceof ASTFloatAdd) {
			ASTFloatAdd fadd = (ASTFloatAdd) e;
			GenEquations(env, fadd.e1, new TypeFloat());
			GenEquations(env, fadd.e2, new TypeFloat());
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTFloatSubtract) {
			GenEquations(env, ((ASTFloatSubtract) e).e1, new TypeFloat());
			GenEquations(env, ((ASTFloatSubtract) e).e2, new TypeFloat());
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTFloatMultiply) {
			GenEquations(env, ((ASTFloatMultiply) e).e1, new TypeFloat());
			GenEquations(env, ((ASTFloatMultiply) e).e2, new TypeFloat());
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTFloatDivide) {
			GenEquations(env, ((ASTFloatDivide) e).e1, new TypeFloat());
			GenEquations(env, ((ASTFloatDivide) e).e2, new TypeFloat());
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTFloatNegative) {
			GenEquations(env, ((ASTFloatNegative) e).e, new TypeFloat());
			listEq.add(new Equation(new TypeFloat(), t));
		} else if (e instanceof ASTTuple) {
			ArrayList<Type> tt = new ArrayList<>();
			for (ASTExpression expression : ((ASTTuple) e).es) {
				tv = new TypeVisitor(env);
				Type tprime = expression.accept(tv);
				GenEquations(env, expression, tprime);
				tt.add(tprime);
			}
			listEq.add(new Equation(new TypeTuple(tt), t));
		} else if (e instanceof ASTLetTuple) {
			ASTLetTuple lt = (ASTLetTuple) e;
			Type tupleType;
			ArrayList<Type> tupleElt = new ArrayList<>();

			for (int i = 0; i < lt.ids.size(); i++) {
				tupleElt.add(env.get(lt.ids.get(i).toString()));
			}

			tupleType = new TypeTuple(tupleElt);
			TypeVisitor tv2 = new TypeVisitor();
			listEq.add(new Equation(tupleType, lt.e1.accept(tv2)));

			GenEquations(tv2.env, lt.e1, tupleType);
			GenEquations(env, lt.e2, t);

			for (int i = 0; i < lt.ids.size(); i++) {
				newkey = lt.ids.get(i).toString();
				if (tv2.env.containsKey(lt.ids.get(i).toString())) {
					while (env.containsKey(newkey)) {
						newkey = newkey.concat("'");
					}
					env.put(newkey, tv2.env.get(lt.ids.get(i).toString()));
				}

			}
		} else if (e instanceof ASTGet) {
			GenEquations(env, ((ASTGet) e).e2, new TypeInteger());
			if (((ASTGet) e).e1.getClass().getSimpleName().equals("ASTVariable")) {
				listEq.add(new Equation(t, ((TypeArray) env.get(((ASTVariable) ((ASTGet) e).e1).id.toString())).t));
			}
		} else if (e instanceof ASTPut) {
			ASTPut p = (ASTPut) e;
			GenEquations(env, p.e2, new TypeInteger());
			GenEquations(env, p.e3, ((TypeArray) env.get(p.e1.toString())).t);
			listEq.add(new Equation(t, new TypeUnit()));
		} else if (e instanceof ASTArray) {
			ASTArray a = (ASTArray) e;
			GenEquations(env, a.e1, new TypeInteger());
			TypeArray ArrType = (TypeArray) a.accept(tv);
			GenEquations(env, a.e2, ArrType.t);
			listEq.add(new Equation(t, ArrType));
		} else if (e instanceof ASTApp) {
			try {
				ASTApp app = (ASTApp) e;

				ArrayList<ASTExpression> argList = new ArrayList<>();
				argList.addAll(app.es);

				ArrayList<Type> initialArgs = new ArrayList<>();

				Type to = app.e.accept(tv);

				if (app.e.getClass().getSimpleName().equals("ASTVariable")) {


					initialArgs.addAll(((TypeFunction) env.get(((ASTVariable) app.e).id.toString())).argType);

					if (to.getClass().getSimpleName().equals(((TypeFunction) env.get(((ASTVariable) app.e).id.toString())).returnType)) {
						listEq.add(new Equation(to, ((TypeFunction) env.get(((ASTVariable) app.e).id.toString())).returnType));
					}


				} else if (app.e.getClass().getSimpleName().equals("ASTGet")) {


					initialArgs.addAll(((TypeFunction) ((TypeArray) env.get(((ASTVariable) ((ASTGet) app.e).e1).id.toString())).t).argType);

					if (to.getClass().getSimpleName().equals(((TypeFunction) ((TypeArray) env.get(((ASTVariable) ((ASTGet) app.e).e1).id.toString())).t).returnType)) {
						listEq.add(new Equation(to, ((TypeFunction) ((TypeArray) env.get(((ASTVariable) ((ASTGet) app.e).e1).id.toString())).t).returnType));
					}

				} else {
					initialArgs.addAll(((TypeFunction) env.get(((ASTApp) app.e).toString())).argType);

					if (to.getClass().getSimpleName().equals(((TypeFunction) env.get(((ASTApp) app.e).toString())).returnType)) {
						listEq.add(new Equation(to, ((TypeFunction) env.get(((ASTApp) app.e).toString())).returnType));
					}

					TypeFunction func = ((TypeFunction) env.get(((ASTVariable) ((ASTApp) app.e).e).id.toString()));

					for (int i = 0; i < argList.size(); i++) {

						listEq.add(new Equation(func.argType.get(i), argList.get(i).accept(tv)));
					}
				}

				int x = 0;

				for (ASTExpression expression : argList) {

					tv = new TypeVisitor(env);
					t1 = expression.accept(tv);

					if (t1.getClass().getSimpleName().equals(initialArgs.get(x).getClass().getSimpleName())) {
						listEq.add(new Equation(t1, initialArgs.get(x)));
					} else if (initialArgs.get(x).getClass().getSimpleName().equals("TypeVariable")) {
						listEq.add(new Equation(t1, initialArgs.get(x)));
					}

					GenEquations(env, expression, initialArgs.get(x));

					x++;

				}

				// And here we match the theoretical and given return types
				if (app.e.getClass().getSimpleName().equals("ASTVariable")) {
					listEq.add(new Equation(((TypeFunction) env.get(((ASTVariable) app.e).id.toString())).returnType, t));
				} else if (app.e.getClass().getSimpleName().equals("ASTGet")) {
					listEq.add(new Equation(((TypeFunction) ((TypeArray) env.get(((ASTVariable) ((ASTGet) app.e).e1).id.toString())).t).returnType, t));
				} else {
					listEq.add(new Equation(((TypeFunction) env.get(((ASTApp) app.e).toString())).returnType, t));
				}

			} catch (IndexOutOfBoundsException indexException) {
				// Exception for when there are too many arguments. Eg., print_int with three arguments
			}
		} else
			throw new NotImplementedException("Type " + e.getClass().getSimpleName() + " is not implemented in typing!");
		// Exception that throws an error for when the parser let something through that hasn't been implemented
		return listEq;
	}

	private static void SolveEquations(ArrayList<Equation> ListEq, HashMap<String, Type> env) {
		ArrayList<Equation> toRemove = new ArrayList<>();
		ArrayList<Equation> toAdd = new ArrayList<>();
		boolean isSolved;

		do {
			isSolved = true;


			// To remove redundant equations (such as int => int or ?5 => ?5) and simplify
			for (Equation e : ListEq) {
				if (e.t1.getClass().getSimpleName().equals("TypeArray") && e.t2.getClass().getSimpleName().equals("TypeArray")) {
					e.t1 = ((TypeArray) e.t1).t;
					e.t2 = ((TypeArray) e.t2).t;
				}

				// If similar, we remove the equation
				if (e.t1.toString().equals(e.t2.toString())) {
					toRemove.add(e);
				}

				if (e.t1.getClass().getSimpleName().equals("TypeTuple") && e.t2.getClass().getSimpleName().equals("TypeTuple") && ((TypeTuple) e.t1).elementTypes.size() == ((TypeTuple) e.t2).elementTypes.size()) {

					for (int i = 0; i < ((TypeTuple) e.t1).elementTypes.size(); i++) {
						if (!((TypeTuple) e.t1).elementTypes.get(i).toString().equals(((TypeTuple) e.t2).elementTypes.get(i).toString())) {
							toAdd.add(new Equation(((TypeTuple) e.t1).elementTypes.get(i), ((TypeTuple) e.t2).elementTypes.get(i)));
						}

					}
					toRemove.add(e);

					isSolved = false;
				}

			}
			// Add all new equations and remove redudants ones
			ListEq.addAll(toAdd);
			ListEq.removeAll(toRemove);

			for (Equation e : ListEq) {

				// If the right part is an unknown variable, we switch them
				if (e.t2.toString().startsWith("?") && e.t2.getClass().getSimpleName().equals("TypeVariable")) {

					Type tprov = e.t2;
					e.t2 = e.t1;
					e.t1 = tprov;
					EquationVisitor ev = new EquationVisitor(tprov, e.t1, env);
					ev.visit(ListEq);
					isSolved = false;

				}

				// If the left part is, we replace it with the right part everywhere
				if (e.t1.toString().startsWith("?") && e.t1.getClass().getSimpleName().equals("TypeVariable")) {

					EquationVisitor ev = new EquationVisitor(e.t1, e.t2, env);
					ev.visit(ListEq);
					isSolved = false;

				}


				// Functions
				if (e.t1.getClass().getSimpleName().equals("TypeFunction") && e.t2.getClass().getSimpleName().equals("TypeFunction")) {

					// For every type in the "from" part
					for (int i = 0; i < ((TypeFunction) e.t1).argType.size(); i++) {

						Type tprime = ((TypeFunction) e.t1).argType.get(i);
						Type tsec = ((TypeFunction) e.t2).argType.get(i);

						if (tprime.getClass().getSimpleName().equals("TypeVariable")) {
							EquationVisitor ev = new EquationVisitor(tprime, tsec, env);
							ev.visit(ListEq);
							isSolved = false;
						}

						if (tsec.getClass().getSimpleName().equals("TypeVariable")) {
							EquationVisitor ev = new EquationVisitor(tsec, tprime, env);
							ev.visit(ListEq);
							isSolved = false;
						}
					}

					if (((TypeFunction) e.t1).returnType.getClass().getSimpleName().equals("TypeVariable")) {
						EquationVisitor ev = new EquationVisitor(((TypeFunction) e.t1).returnType, ((TypeFunction) e.t2).returnType, env);
						ev.visit(ListEq);
						isSolved = false;
					}

					if (((TypeFunction) e.t2).returnType.getClass().getSimpleName().equals("TypeVariable")) {
						EquationVisitor ev = new EquationVisitor(((TypeFunction) e.t2).returnType, ((TypeFunction) e.t1).returnType, env);
						ev.visit(ListEq);
						isSolved = false;
					}

				}

			}
		} while (!isSolved);
		/*
		 * Typing error if (!ListEq.isEmpty()) { }
		 */
	}

	public static ArrayList<Equation> getListEq() {
		return listEq;
	}
}
