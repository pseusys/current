package team.lmao.ast.visitors;

import java.util.*;

import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;
import team.lmao.utils.Logger;

public class AlphaConversionVisitor implements Visitor<Void> {
	Logger logger;
	Map<Id, Id> variables = new HashMap<>();

	public AlphaConversionVisitor(Logger logger) {
		this.logger = logger;
	}

	public Void visit(ASTUnit e) {
		return null;
	}

	public Void visit(ASTBoolean e) {
		return null;
	}

	public Void visit(ASTInteger e) {
		return null;
	}

	public Void visit(ASTFloat e) {
		return null;
	}

	public Void visit(ASTBooleanNot e) {
		e.e.accept(this);
		return null;
	}

	public Void visit(ASTIntegerNegative e) {
		e.e.accept(this);
		return null;
	}

	public Void visit(ASTIntegerAdd e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTIntegerSubtract e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTFloatNegative e) {
		e.e.accept(this);
		return null;
	}

	public Void visit(ASTFloatAdd e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTFloatSubtract e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTFloatMultiply e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTFloatDivide e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTIntegerEqual e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTIntegerLessOrEqual e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTIf e) {
		e.e1.accept(this);
		e.e2.accept(this);
		e.e3.accept(this);
		return null;
	}

	@Override
	public Void visit(ASTFunctionDefine e) {
		return null;
	}

	public Void visit(ASTLet e) {
		Id oldId = e.id;
		if (findUsingIteratorMap(e.id, variables) && (getKey(e.id, variables) == null)) {
			while (findUsingIteratorMap(e.id, variables)) {
				e.id = e.id.gen2();
			}
			remove(oldId, variables);
			variables.put(oldId, e.id);

		} else if (findUsingIteratorMap(e.id, variables) && !(getKey(e.id, variables) == null)) {
			e.id = getKey(e.id, variables);
		} else {
			variables.put(e.id, null);
		}

		e.e1.accept(this);
		// We reset the value with which the bound variable needs to be replaced
		if (!(getKey(oldId, variables) == null)) {
			remove(oldId, variables);
			variables.put(oldId, null);
		}

		e.e2.accept(this);
		return null;
	}

	// print all the variable outside of a let
	public Void visit(ASTVariable e) {
		if (findUsingIteratorMap(e.id, variables) && !(getKey(e.id, variables) == null)) {
			e.id = getKey(e.id, variables);
		}
		return null;
	}

	public Void visit(ASTLetRec e) {
		List<Id> liste = new ArrayList<>();
		Iterator<Id> iterator_arg_let = e.fd.args.iterator();
		Iterator<Id> iterator2 = e.fd.args.iterator();
		while (iterator_arg_let.hasNext()) {
			Id variable = iterator_arg_let.next();
			Id oldId = variable;
			if (findUsingIteratorMap(variable, variables) && getKey(variable, variables) == null) {
				while (findUsingIteratorMap(variable, variables)) {
					variable = variable.gen2();
				}
				remove(oldId, variables);
				variables.put(oldId, variable);
			} else if (findUsingIteratorMap(variable, variables) && !(getKey(variable, variables) == null)) {
				variable = getKey(variable, variables);
			} else {
				variables.put(variable, null);
			}
			liste.add(variable);
			e.fd.args = liste;
		}
		e.fd.e.accept(this);

		while (iterator2.hasNext()) {
			Id variable = iterator2.next();
			if (!(getKey(variable, variables) == null)) {
				remove(variable, variables);
				variables.put(variable, null);
			}
		}

		e.e.accept(this);
		return null;
	}

	public Void visit(ASTApp e) {
		e.e.accept(this);
		return null;
	}

	public Void visit(ASTTuple e) {
		return null;
	}

	public Void visit(ASTLetTuple e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTArray e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTGet e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return null;
	}

	public Void visit(ASTPut e) {
		e.e1.accept(this);
		e.e2.accept(this);
		e.e3.accept(this);
		return null;
	}



	public boolean findUsingIteratorMap(Id id, Map<Id, Id> map) {
		for (Map.Entry<Id, Id> entry : map.entrySet()) {
			Id OldId = entry.getKey();
			if (OldId.equals(id)) {
				return true;
			}
		}
		return false;
	}


	public void remove(Id id, Map<Id, Id> map) {
		for (Iterator<Map.Entry<Id, Id>> i = map.entrySet().iterator(); i.hasNext();) {
			Map.Entry<Id, Id> entry = i.next();
			if (entry.getKey().equals(id)) {

				i.remove();
				return;
			}

		}
	}

	public Id getKey(Id id, Map<Id, Id> map) {
		for (Map.Entry<Id, Id> entry : map.entrySet())
			if (entry.getKey().equals(id))
				return entry.getValue();
		return null;
	}

	@Override
	public Void visit(ASTCallClosure e) {
		return null;
	}

	@Override
	public Void visit(ASTClosure e) {
		return null;
	}
}
