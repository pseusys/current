package team.lmao.ast.visitors;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;
import team.lmao.asml.structures.ASMLProgram.ASMLFunction;
import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;
import team.lmao.exceptions.FileException;
import team.lmao.utils.Logger;
import team.lmao.utils.Pair;

public class ASMLgeneration implements Visitor<ASTExpression> {

	public static List<Pair<ASMLLabel, ASMLFloatLiteral>> global_declaration = new ArrayList<>();
	public static List<Pair<ASMLFunction, ASMLBlock>> functions_declaration = new ArrayList<>();

	// List of Let that will be used to create ASMLBlocks
	List<ASMLAssignment> assignements_main = new ArrayList<>();
	List<ASMLAssignment> assignements_function = new ArrayList<>();
	List<ASMLAssignment> assignements_if = new ArrayList<>();

	// Variables used to store the right part of the Let
	ASMLExpression exp_main = null;
	ASMLExpression exp_function = null;
	ASMLExpression exp_if = null;

	// Booleans to know in which block we are working
	static boolean in_function = false;
	boolean in_if = false;
	boolean start = true;
	boolean is_final = false;

	// Different ASMLBlock : one for functions, one for the main code, one for if blocks
	public static ASMLBlock main_code;
	ASMLBlock block_function;
	ASMLBlock block_if;

	Logger logger;
	List<ASTFunctionDefine> liste;

	List<Id> current_arguments;

	int count = 0;
	int memory = 4;
	int nested_if = 0;
	private static int MEMORY_STATE = 0;
	ASMLLabel label_closure = new ASMLLabel("id");

	ASMLCallLabel closure_call = null;
	List<Pair<ASMLLabel, ASMLLabel>> variables = new ArrayList<>();
	List<Id> current_arg = new ArrayList<>();

	// Fonction used for debug
	public ASMLgeneration(Logger logger, List<ASTFunctionDefine> list) {
		this.logger = logger;
		this.liste = list;
	}

	// Function used to return the obtained program
	public static ASMLProgram constructProgram() {
		return new ASMLProgram(global_declaration, functions_declaration, main_code);
	}

	public ASMLgeneration(List<ASTFunctionDefine> list) {
		this.liste = list;
	}

	public void returnFunctionDefinition(List<ASTFunctionDefine> list) {
		for (ASTFunctionDefine exp : list) {
			ASMLLabel label = new ASMLLabel(exp.label);
			List<ASMLIdentifier> arguments = new ArrayList<>();
			ListIterator<Id> liste = exp.args.listIterator();
			ASMLIdentifier arg;
			current_arg = exp.args;
			while (liste.hasNext()) {
				arg = new ASMLIdentifier(liste.next().toString());
				arguments.add(arg);
			}
			ASMLFunction function = new ASMLFunction(label, arguments);
			in_function = true;

			Pair<ASMLLabel, ASMLLabel> pair = new Pair<>(label, null);
			variables.add(pair);
			exp.e.accept(this);

			Iterator<Pair<ASMLLabel, ASMLLabel>> iterator = variables.iterator();
			List<Pair<ASMLLabel, ASMLLabel>> variables_eph = new ArrayList<>();
			while (iterator.hasNext()) {
				Pair<ASMLLabel, ASMLLabel> var = iterator.next();
				if (var.left.toString() == label.toString()) {
					if (var.right != null) {
						variables_eph.add(var);
					}
				} else {
					variables_eph.add(var);
				}
			}
			variables = variables_eph;
			Pair<ASMLFunction, ASMLBlock> pair2 = new Pair<>(function, block_function);
			functions_declaration.add(pair2);

			assignements_function = new ArrayList<>();
			block_function = null;
			in_function = false;
		}
		return;
	}

	public ASTExpression visit(ASTUnit e) {
		return e;
	}

	public ASTExpression visit(ASTBoolean e) {
		return e;
	}

	public ASTExpression visit(ASTInteger e) {

		if (in_function && !in_if) {
			exp_function = new ASMLIntegerLiteral(e.i);

		} else if (in_if) {
			exp_if = new ASMLIntegerLiteral(e.i);

		} else {
			exp_main = new ASMLIntegerLiteral(e.i);
		}
		return e;
	}

	// Function used to return the value of an integer
	public int returnInt(ASTInteger e) {
		return e.i;
	}

	public ASTExpression visit(ASTFloat e) {

		// Addding every float we find to the float declaration list
		ASMLLabel label = new ASMLLabel(ASTFloat.gen());
		ASMLFloatLiteral literal = new ASMLFloatLiteral(e.f);
		Pair<ASMLLabel, ASMLFloatLiteral> pair = new Pair<>(label, literal);
		global_declaration.add(pair);

		// Adding the corresponding memory declaration to access the float
		ASMLIdentifier expression = new ASMLIdentifier(ASTFloat.gen2());
		ASMLIdentifierOrImmediate expression2 = new ASMLIdentifier(String.valueOf(0));
		ASMLAssignment pair2 = new ASMLAssignment(expression, label);

		if (in_function && !in_if) {

			exp_function = new ASMLMemoryRetrieval(expression, expression2);
			assignements_function.add(pair2);
		} else if (in_if) {
			exp_if = new ASMLMemoryRetrieval(expression, expression2);
			assignements_if.add(pair2);
		} else {

			exp_main = new ASMLMemoryRetrieval(expression, expression2);
			assignements_main.add(pair2);
		}

		return e;
	}

	public ASTExpression visit(ASTBooleanNot e) {
		e.e.accept(this);
		return e;
	}

	public ASTExpression visit(ASTIntegerNegative e) {

		ASMLNegativeIdentifier exp = null;
		ASMLIdentifier id = null;
		if (e.e.accept(this) instanceof ASTInteger) {
			int i = returnInt((ASTInteger) e.e);
			id = new ASMLIdentifier(String.valueOf(i));
		} else if (e.e.accept(this) instanceof ASTExpression) {
			id = new ASMLIdentifier(returnId((ASTVariable) e.e).toString());

		}

		exp = new ASMLNegativeIdentifier(id);

		if (in_function && !in_if) {
			exp_function = exp;
		} else if (in_function) {
			exp_if = exp;
		} else {
			exp_main = exp;
		}

		return e;
	}

	public ASTExpression visit(ASTIntegerAdd e) {
		ASMLIdentifierOrImmediate id2 = null;
		ASMLIdentifier id1 = null;
		if (e.e1.accept(this) instanceof ASTVariable) {
			id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		} else if (e.e1.accept(this) instanceof ASTInteger) {
			id1 = new ASMLIdentifier(String.valueOf(returnInt((ASTInteger) e.e1)));
		} else {
			throw new FileException("wrong type");
		}
		if (e.e2.accept(this) instanceof ASTVariable) {
			id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());
		} else if (e.e2.accept(this) instanceof ASTInteger) {
			int i = returnInt((ASTInteger) e.e2);
			id2 = new ASMLIdentifier(String.valueOf(i));
		} else {
			throw new FileException("wrong type");
		}
		if (in_function && !in_if) {
			exp_function = new ASMLAddIdentifierOrImmediate(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLAddIdentifierOrImmediate(id1, id2);
		} else {
			exp_main = new ASMLAddIdentifierOrImmediate(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTIntegerSubtract e) {

		ASMLIdentifier id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		ASMLIdentifier id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());

		if (in_function && !in_if) {
			exp_function = new ASMLSubtractIdentifierOrImmediate(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLSubtractIdentifierOrImmediate(id1, id2);
		} else {
			exp_main = new ASMLSubtractIdentifierOrImmediate(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTFloatNegative e) {

		if (in_function && !in_if) {
			exp_function = new ASMLNegativeFloat((ASMLIdentifier) e.e);
		} else if (in_if) {
			exp_if = new ASMLNegativeFloat((ASMLIdentifier) e.e);
		} else {
			exp_main = new ASMLNegativeFloat((ASMLIdentifier) e.e);
		}
		e.e.accept(this);
		return e;
	}

	public ASTExpression visit(ASTFloatAdd e) {

		ASMLIdentifier id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		ASMLIdentifier id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());

		if (in_function && !in_if) {
			exp_function = new ASMLAddFloat(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLAddFloat(id1, id2);
		} else {
			exp_main = new ASMLAddFloat(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTFloatSubtract e) {

		ASMLIdentifier id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		ASMLIdentifier id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());

		if (in_function && !in_if) {
			exp_function = new ASMLSubtractFloat(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLSubtractFloat(id1, id2);
		} else {
			exp_main = new ASMLSubtractFloat(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTFloatMultiply e) {

		ASMLIdentifier id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		ASMLIdentifier id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());

		if (in_function && !in_if) {
			exp_function = new ASMLMultiplyFloat(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLMultiplyFloat(id1, id2);
		} else {
			exp_main = new ASMLMultiplyFloat(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTFloatDivide e) {

		ASMLIdentifier id1 = new ASMLIdentifier(returnId((ASTVariable) e.e1).toString());
		ASMLIdentifier id2 = new ASMLIdentifier(returnId((ASTVariable) e.e2).toString());

		if (in_function && !in_if) {
			exp_function = new ASMLDivideFloat(id1, id2);
		} else if (in_if) {
			exp_if = new ASMLDivideFloat(id1, id2);
		} else {
			exp_main = new ASMLDivideFloat(id1, id2);
		}
		return e;
	}

	public ASTExpression visit(ASTIntegerEqual e) {
		return e;
	}

	public ASTExpression visit(ASTIntegerLessOrEqual e) {
		return e;
	}

	public ASTExpression visit(ASTIf e) {
		// We put in_if to true to update if global variables only
		in_if = true;

		// We check if if are nested
		nested_if = nested_if + 1;

		// Initialization of variables
		ASMLExpression exp = null;
		ASMLIdentifier comp1 = null;
		ASMLIdentifier comp2 = null;

		// This boolean avoids to test e.e1.accept(this) twice
		boolean is_IntegerEqual = false;

		// We check that it is indeed a boolean
		if (e.e1.accept(this) instanceof ASTIntegerEqual || e.e1.accept(this) instanceof ASTIntegerLessOrEqual) {

			Pair<ASTExpression, ASTExpression> composantes = null;

			if (e.e1.accept(this) instanceof ASTIntegerEqual) {
				is_IntegerEqual = true;
				composantes = returnCompEqual((ASTIntegerEqual) e.e1);
			} else {
				composantes = returnComp((ASTIntegerLessOrEqual) e.e1);
			}

			if (composantes.left instanceof ASTInteger) {
				ASTInteger ent = (ASTInteger) composantes.left;
				comp1 = new ASMLIdentifier(String.valueOf(ent.i));
			} else if (composantes.left instanceof ASTVariable) {
				ASTVariable var = (ASTVariable) composantes.left;
				comp1 = new ASMLIdentifier(var.id.toString());
			} else {
				logger.debugln("Non-defined yet");
			}

			if (composantes.right instanceof ASTInteger) {
				ASTInteger ent = (ASTInteger) composantes.right;
				comp2 = new ASMLIdentifier(String.valueOf(ent.i));
			} else if (composantes.right instanceof ASTVariable) {
				ASTVariable var = (ASTVariable) composantes.right;
				comp2 = new ASMLIdentifier(var.id.toString());
			} else {
				logger.debugln("Non-defined yet");
			}
		} else {
			throw new FileException("doesn't follow the structure if boollean");
		}

		// We go through e2 and e3 to create the two ASMLBlocks
		Id var = new Id("w" + String.valueOf(count));
		ASTVariable variable = new ASTVariable(var);
		ASMLBlock block = null;
		if (e.e2.accept(this) instanceof ASTLet) {
			block = block_if;
		} else {
			ASMLIdentifier label_if = new ASMLIdentifier(variable.id.toString());
			e.e2.accept(this);
			ASMLAssignment pair_if = new ASMLAssignment(label_if, exp_if);
			assignements_if.add(pair_if);
			ASMLExpression expfinal_if = (ASMLExpression) label_if;
			block = new ASMLBlock(assignements_if, expfinal_if);
			count++;
		}
		// We reset everything before going through the second block
		assignements_if = new ArrayList<>();
		block_if = null;

		Id var1 = new Id("w" + String.valueOf(count));
		ASTVariable variable1 = new ASTVariable(var1);

		if (e.e3.accept(this) instanceof ASTLet) {
		} else {
			ASMLIdentifier label_if = new ASMLIdentifier(variable1.id.toString());
			e.e3.accept(this);
			ASMLAssignment pair_if = new ASMLAssignment(label_if, exp_if);
			if (exp_if != null) {
				assignements_if.add(pair_if);
			}
			ASMLExpression expfinal_if = (ASMLExpression) variable1;
			block_if = new ASMLBlock(assignements_if, expfinal_if);
			count++;
		}

		if (is_IntegerEqual) {
			exp = new ASMLIfIdentifierOrImmediateEqualElse(comp1, (ASMLIdentifierOrImmediate) comp2, block, block_if);
		} else {
			exp = new ASMLIfIdentifierOrImmediateLessOrEqualElse(comp1, (ASMLIdentifierOrImmediate) comp2, block, block_if);
		}

		if (nested_if > 1) {
			if (in_function && !in_if) {
				exp_function = exp;
			} else if (in_if) {
				exp_if = exp;
			} else {
				exp_main = exp;
			}
			in_if = false;
		} else {

			in_if = false;
			if (in_function) {
				exp_function = exp;
			} else {
				exp_main = exp;
			}

		}
		return e;
	}

	public Pair<ASTExpression, ASTExpression> returnCompEqual(ASTIntegerEqual e) {
		Pair<ASTExpression, ASTExpression> pair_comp = new Pair<>(e.e1.accept(this), e.e2.accept(this));
		return pair_comp;
	}


	public Pair<ASTExpression, ASTExpression> returnComp(ASTIntegerLessOrEqual e) {
		Pair<ASTExpression, ASTExpression> pair_comp = new Pair<>(e.e1.accept(this), e.e2.accept(this));
		return pair_comp;
	}

	public ASTExpression visit(ASTLet e) {
		if (start) {
			start = false;
			returnFunctionDefinition(liste);
		}
		if (in_function && !in_if) {
			ASMLIdentifier label_function = new ASMLIdentifier(e.id.toString());
			e.e1.accept(this);
			ASMLAssignment pair_function = new ASMLAssignment(label_function, exp_function);
			assignements_function.add(pair_function);
			is_final = true;
			e.e2.accept(this);
			is_final = false;
			ASMLExpression expfinal_function = exp_function;
			block_function = new ASMLBlock(assignements_function, expfinal_function);
			memory = 4;
		}

		else if (in_if) {
			ASMLIdentifier label_if = new ASMLIdentifier(e.id.toString());

			e.e1.accept(this);
			ASMLAssignment pair_if = new ASMLAssignment(label_if, exp_if);
			assignements_if.add(pair_if);

			e.e2.accept(this);
			ASMLExpression expfinal_if = exp_if;
			block_if = new ASMLBlock(assignements_if, expfinal_if);

		} else {
			ASMLIdentifier label = new ASMLIdentifier(e.id.toString());

			e.e1.accept(this);
			ASMLAssignment pair = new ASMLAssignment(label, exp_main);
			if (exp_main != null) {
				assignements_main.add(pair);
			}

			e.e2.accept(this);
			ASMLExpression expfinal = exp_main;
			main_code = new ASMLBlock(assignements_main, expfinal);
		}

		return e;
	}

	public ASTExpression visit(ASTVariable e) {

		if (in_function && !in_if) {
			ListIterator<Id> liste = current_arg.listIterator();
			boolean in_list = false;
			while (liste.hasNext()) {
				if (liste.next().toString().equals(e.id.toString())) {
					in_list = true;
				}
			}
			if (in_list || is_final) {
				exp_function = (ASMLIdentifierOrImmediate) new ASMLIdentifier(e.id.toString());
			} else {
				ASMLIdentifierOrImmediate id = (ASMLIdentifierOrImmediate) new ASMLIdentifier(String.valueOf(memory));
				exp_function = new ASMLMemoryRetrieval(new ASMLIdentifier("%self"), id);
				memory = memory + 4;

			}
		}

		else if (in_if) {
			exp_if = (ASMLIdentifierOrImmediate) new ASMLIdentifier(e.id.toString());
		} else {
			exp_main = (ASMLIdentifierOrImmediate) new ASMLIdentifier(e.id.toString());
		}
		return e;
	}

	public static Id returnId(ASTVariable e) {
		return e.id;
	}

	// Probably isn't accessed
	public ASTExpression visit(ASTLetRec e) {
		e.fd.e.accept(this);
		e.e.accept(this);
		return e;
	}

	public ASTExpression visit(ASTApp e) {
		ASMLLabel id1 = new ASMLLabel(returnId((ASTVariable) e.e).toString());
		Iterator<ASTExpression> iterator = e.es.iterator();
		List<ASMLIdentifier> liste = new ArrayList<>();
		while (iterator.hasNext()) {
			ASTExpression exp = iterator.next();
			ASMLIdentifier var = null;

			if (exp instanceof ASTInteger) {
				int variable = returnInt((ASTInteger) exp);
				var = new ASMLIdentifier(String.valueOf(variable));
			} else {
				Id variable = returnId((ASTVariable) exp);
				var = new ASMLIdentifier(variable.toString());
			}
			liste.add(var);
		}
		if (in_function && !in_if) {
			exp_function = new ASMLCallLabel(id1, liste);
		} else if (in_if) {
			exp_if = new ASMLCallLabel(id1, liste);
		} else {
			exp_main = new ASMLCallLabel(id1, liste);
		}
		return e;
	}

	public ASTExpression visit(ASTTuple e) {
		return e;
	}

	public ASTExpression visit(ASTLetTuple e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return e;
	}

	public ASTExpression visit(ASTArray e) {
		List<ASMLIdentifier> liste = new ArrayList<>();
		ASTExpression exp1 = e.e1.accept(this);
		ASTExpression exp2 = e.e2.accept(this);

		if (exp1 instanceof ASTVariable) {
			ASTVariable exp1bis = (ASTVariable) exp1;
			ASMLIdentifier id1 = new ASMLIdentifier(exp1bis.id.toString());
		} else if (exp1 instanceof ASTInteger) {
			ASMLIdentifier id1 = new ASMLIdentifier(String.valueOf(returnInt((ASTInteger) exp1)));
		}
		if (exp2 instanceof ASTVariable) {
			ASTVariable exp2bis = (ASTVariable) exp2;
			ASMLIdentifier id2 = new ASMLIdentifier(exp2bis.id.toString());
		} else if (exp2 instanceof ASTInteger) {
			ASMLIdentifier id2 = new ASMLIdentifier(String.valueOf(returnInt((ASTInteger) exp2)));
		}


		if (in_function && !in_if) {
			exp_function = new ASMLCallLabel(new ASMLLabel("_min_caml_create_array"), liste);
		} else if (in_if) {
			exp_if = new ASMLCallLabel(new ASMLLabel("_min_caml_create_array"), liste);
		} else {
			exp_main = new ASMLCallLabel(new ASMLLabel("_min_caml_create_array"), liste);
		}
		return e;
	}

	public ASTExpression visit(ASTGet e) {
		e.e1.accept(this);
		e.e2.accept(this);
		return e;
	}

	public ASTExpression visit(ASTPut e) {
		e.e1.accept(this);
		e.e2.accept(this);
		e.e3.accept(this);
		return e;
	}

	public ASTExpression visit(ASTClosure e) {
		ASMLIdentifier iden = new ASMLIdentifier(e.id.toString());
		List<ASMLIdentifier> arguments = new ArrayList<>();
		ListIterator<Id> liste = e.args.listIterator();
		ASMLIdentifier arg;

		ASMLLabel label = new ASMLLabel(e.id.toString());
		Iterator<Pair<ASMLLabel, ASMLLabel>> iterator = variables.iterator();
		while (iterator.hasNext()) {
			Pair<ASMLLabel, ASMLLabel> var = iterator.next();
			if (var.left.toString().equals(label.toString())) {
				label_closure = var.right;
			}
		}

		int lastIdx = variables.size() - 1;
		Pair<ASMLLabel, ASMLLabel> variable = variables.remove(lastIdx);
		Pair<ASMLLabel, ASMLLabel> pair_label = new Pair<>(variable.left, label);
		variables.add(pair_label);

		while (liste.hasNext()) {
			arg = new ASMLIdentifier(liste.next().toString());
			arguments.add(arg);
		}
		if (in_function && !in_if) {
			exp_function = new ASMLCallClosureWithIdentifier(iden, arguments);
		} else if (in_if) {
			exp_if = new ASMLCallClosureWithIdentifier(iden, arguments);
		} else {
			exp_main = new ASMLCallClosureWithIdentifier(iden, arguments);
		}
		return e;
	}

	public ASTExpression visit(ASTCallClosure e) {
		ASMLLabel label = new ASMLLabel(label_closure.toString());
		ListIterator<Id> liste = e.args.listIterator();
		List<ASMLIdentifier> labels = new ArrayList<>();
		labels.add(new ASMLIdentifier("addr" + label));
		MEMORY_STATE = 0;
		while (liste.hasNext()) {
			MEMORY_STATE = MEMORY_STATE + 4;
			labels.add(new ASMLIdentifier(liste.next().toString()));
		}
		String name = (label_closure.toString().charAt(0) == '_') ? label_closure.toString().substring(1) : (label_closure.toString());
		ASMLNewPlacement place = new ASMLNewPlacement(new ASMLIdentifier(String.valueOf(MEMORY_STATE + 4)));
		ASMLAssignment pair_main = new ASMLAssignment(new ASMLIdentifier(name), place);
		assignements_main.add(pair_main);
		ASMLAssignment pair1 = new ASMLAssignment(new ASMLIdentifier("addr" + label), label);
		assignements_main.add(pair1);

		for (int i = 0; i <= MEMORY_STATE / 4; i++) {
			ASMLMemoryAssignment ass = new ASMLMemoryAssignment(new ASMLIdentifier(name), new ASMLIdentifier(String.valueOf(i * 4)), new ASMLIdentifier(labels.get(i).getName()));
			ASMLAssignment pair = new ASMLAssignment(new ASMLIdentifier(ASMLMemoryAssignment.gen()), ass);
			assignements_main.add(pair);
		}

		if (in_function && !in_if) {
			exp_function = closure_call;
		} else if (in_if) {
			exp_if = closure_call;
		} else {
			exp_main = closure_call;
		}
		return null;
	}

	@Override
	public ASTExpression visit(ASTFunctionDefine e) {

		List<ASMLIdentifier> arguments = new ArrayList<>();
		ListIterator<Id> liste = e.args.listIterator();
		ASMLIdentifier arg;
		while (liste.hasNext()) {
			arg = new ASMLIdentifier(liste.next().toString());
			arguments.add(arg);
		}

		boolean closure = false;
		ASMLLabel label = new ASMLLabel(e.id.toString());
		Iterator<Pair<ASMLLabel, ASMLLabel>> iterator = variables.iterator();


		while (iterator.hasNext()) {
			Pair<ASMLLabel, ASMLLabel> var = iterator.next();
			if (var.left.toString().equals(label.toString())) {
				closure = true;
			}
		}

		if (closure) {
			closure_call = new ASMLCallLabel(new ASMLLabel(e.id.toString()), arguments);
		} else {
			exp_main = new ASMLCallLabel(new ASMLLabel(e.id.toString()), arguments);
		}
		closure = false;

		return null;
	}

	public ASMLLabel getKey(ASMLLabel label, Map<ASMLLabel, ASMLLabel> map) {
		for (Map.Entry<ASMLLabel, ASMLLabel> entry : map.entrySet())
			if (entry.getKey().equals(label))
				return entry.getValue();
		return null;
	}

	public void remove(ASMLLabel label, Map<ASMLLabel, ASMLLabel> map) {
		for (Iterator<Map.Entry<ASMLLabel, ASMLLabel>> i = map.entrySet().iterator(); i.hasNext();) {
			Map.Entry<ASMLLabel, ASMLLabel> entry = i.next();
			if (entry.getKey().equals(label)) {

				i.remove();
				return;
			}

		}
	}
}
