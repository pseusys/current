package team.lmao.ast.visitors;

import java.util.*;

import team.lmao.ast.typechecking.Equation;
import team.lmao.ast.types.*;
import team.lmao.ast.types.Type;

public class EquationVisitor {

	Type t1;
	Type t2;
	HashMap<String, Type> env;

	public EquationVisitor(Type t1, Type t2, HashMap<String, Type> env) {
		this.t1 = t1;
		this.t2 = t2;
		this.env = env;
	}

	public boolean visit(ArrayList<Equation> le) {
		if (t1.toString().equals(t2.toString())) {
			return false;
		}
		for (Equation e : le) {
			this.visit(e);
		}
		replaceInEnv();
		return false;
	}

	public boolean visit(Equation e) {
		if (e.t1.accept(this)) {
			e.t1 = t2;
		}

		if (e.t2.accept(this)) {
			e.t2 = t2;
		}
		return false;
	}

	public boolean visit(TypeUnit e) {
		return false;
	}

	public boolean visit(TypeBoolean e) {
		return false;
	}

	public boolean visit(TypeInteger e) {
		return false;
	}

	public boolean visit(TypeFloat e) {
		return false;
	}

	public boolean visit(TypeVariable e) {

		if (e.toString().equals(t1.toString())) {
			return true;
		}

		return false;
	}

	public boolean visit(TypeFunction e) {
		for (int i = 0; i < e.argType.size(); i++) {
			e.argType.get(i).accept(this);

			if (e.argType.get(i).accept(this)) {
				if (!e.argType.getClass().getSimpleName().equals("TypeFunction")) {

					Type targType = t2;
					e.argType.add(targType);
					targType = e.argType.remove(i);
				}
				e.argType.set(i, t2);
			}

			if (e.returnType.accept(this)) {

				e.returnType = t2;
				Type targType = e.argType.get(i);

				if (e.returnType.getClass().getSimpleName().equals("TypeFunction")) {

					e.argType.set(i, ((TypeFunction) e.returnType).argType.get(i));
					e.argType.add(targType);
					e.returnType = ((TypeFunction) e.returnType).returnType;
				}

			}
		}
		return false;

	}

	public boolean visit(TypeTuple e) {
		for (Type t : e.elementTypes) {

			if (t.accept(this)) {
				t = t2;
			}
		}
		return false;
	}

	public boolean visit(TypeArray e) {
		if (e.t.accept(this)) {
			e.t = t2;
		}
		return false;
	}

	public boolean replaceInEnv() {
		Set<Map.Entry<String, Type>> envSet = env.entrySet();
		Iterator<Map.Entry<String, Type>> it = envSet.iterator();
		while (it.hasNext()) {

			Map.Entry<String, Type> e = it.next();
			if (e.getValue().toString().equals(t1.toString())) {
				String key = (String) e.getKey();
				env.put(key, t2);
			}
			switch (e.getValue().getClass().getSimpleName()) {
				case "TypeFunction":

				case "TypeTuple":

				case "TypeArray":
			}
		}
		return false;
	}

	public boolean replaceInFunction(TypeFunction f) {
		switch (f.returnType.getClass().getSimpleName()) {
			case "TypeFunction":
				replaceInFunction((TypeFunction) f.returnType);
				break;
			case "TypeTuple":
				replaceInTuple((TypeTuple) f.returnType);
				break;
			case "TypeArray":
				replaceInArray((TypeArray) f.returnType);
				break;
		}
		if (f.returnType.toString().equals(t1.toString())) {
			f.returnType = t2;
		}
		for (int i = 0; i < f.argType.size(); i++) {

			switch (f.argType.get(i).getClass().getSimpleName()) {
				case "TypeFunction":
					replaceInFunction((TypeFunction) f.argType.get(i));
					break;

				case "TypeTuple":
					replaceInTuple((TypeTuple) f.argType.get(i));
					break;

				case "TypeArray":
					replaceInArray((TypeArray) f.argType.get(i));
					break;
			}
			if (f.argType.get(i).toString().equals(t1.toString())) {
				f.argType.set(i, t2);
			}
		}

		return false;


	}

	public boolean replaceInTuple(TypeTuple t) {
		for (int i = 0; i < t.elementTypes.size(); i++) {

			switch (t.elementTypes.get(i).getClass().getSimpleName()) {
				case "TypeFunction":
					replaceInFunction((TypeFunction) t.elementTypes.get(i));
					break;

				case "TypeTuple":
					replaceInTuple((TypeTuple) t.elementTypes.get(i));
					break;

				case "TypeArray":
					replaceInArray((TypeArray) t.elementTypes.get(i));
					break;

			}

			if (t.elementTypes.get(i).toString().equals(t1.toString())) {
				t.elementTypes.set(i, t2);
			}

		}

		return false;

	}

	public boolean replaceInArray(TypeArray arr) {
		if (arr.t.toString().equals(t1.toString())) {
			arr.t = t2;
		}

		switch (arr.t.getClass().getSimpleName()) {
			case "TypeFunction":
				replaceInFunction((TypeFunction) arr.t);
				break;

			case "TypeTuple":
				replaceInTuple((TypeTuple) arr.t);
				break;

			case "TypeArray":
				replaceInArray((TypeArray) arr.t);
				break;
		}

		return false;

	}
}
