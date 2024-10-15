package team.lmao.asml.visitors;

import java.util.*;
import java.util.stream.Collectors;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBase;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;
import team.lmao.exceptions.IllegalStateException;
import team.lmao.utils.Pair;

public class RegisterAllocationVisitor implements Visitor<List<ASMLBase>> {
	private static final int CALL_PARAMETERS_OFFSET = 9 * 4;

	private int offset;
	private final CopyVisitor copier = new CopyVisitor();
	private final Map<String, Integer> variableMemoryOffsets = new HashMap<>();
	private final Map<String, ASMLRegister> variableRegisters = new HashMap<>(); // TODO: for future optimizations.

	public RegisterAllocationVisitor() {
		this(0);
	}

	private RegisterAllocationVisitor(int offset) {
		this.offset = offset;
	}

	private ASMLExpression getVariable(ASMLIdentifierOrImmediate operator) {
		if (operator instanceof ASMLIntegerLiteral)
			return operator;
		String name = ((ASMLIdentifier) operator).getName();
		if (name.equals(ASMLLabel.SPECIAL)) {
			return new ASMLMemoryRetrieval(ASMLRegister.STACK_FRAME_REGISTER, new ASMLIntegerLiteral(CALL_PARAMETERS_OFFSET));
		} else if (variableRegisters.containsKey(name))
			return variableRegisters.get(name);
		else if (variableMemoryOffsets.containsKey(name))
			return new ASMLMemoryRetrieval(ASMLRegister.STACK_FRAME_REGISTER, new ASMLIntegerLiteral(-variableMemoryOffsets.get(name) - ASMLRegister.REGISTER_SIZE));
		else
			return new ASMLMemoryRetrieval((ASMLIdentifier) operator, new ASMLIntegerLiteral(0));
	}

	@Override
	public List<ASMLBase> visit(ASMLAddFloat e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLAddFloat(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLAddIdentifierOrImmediate e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLAddIdentifierOrImmediate(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLAssignment e) {
		List<ASMLBase> expression = new ArrayList<>((e.right instanceof ASMLIntegerLiteral) ? List.of(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, e.right)) : e.right.accept(this));
		expression.add(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, new ASMLPushRegister(ASMLRegister.RESERVED_REGISTER)));
		variableMemoryOffsets.put(e.left.getName(), offset);
		offset += ASMLRegister.REGISTER_SIZE;
		return expression;
	}

	@Override
	public List<ASMLBase> visit(ASMLCallClosureWithIdentifier e) {
		List<ASMLBase> assignments = new ArrayList<>(List.of(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, getVariable(e.identifier)), new ASMLAssignment(ASMLRegister.CLOSURE_REGISTER, new ASMLMemoryRetrieval(ASMLRegister.RESERVED_REGISTER, new ASMLIntegerLiteral(0)))));

		for (int i = 0; i < e.arguments.size(); i++) {
			if (i <= ASMLRegister.LAST_ARGUMENTS_REGISTER_NUMBER) {
				ASMLRegister register = new ASMLRegister(ASMLRegister.FIRST_ARGUMENTS_REGISTER_NUMBER + i);
				assignments.add(new ASMLAssignment(register, getVariable(e.arguments.get(i))));
			} else
				throw new IllegalStateException("closure '" + e.identifier.getName() + "' accepts " + e.arguments.size() + "arguments");
		}

		assignments.add(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, new ASMLCallClosureWithIdentifier(ASMLRegister.RESERVED_REGISTER, List.of())));
		return assignments;
	}

	@Override
	public List<ASMLBase> visit(ASMLCallLabel e) {
		List<ASMLBase> assignments = new ArrayList<>();

		for (int i = 0; i < e.arguments.size(); i++) {
			if (i <= ASMLRegister.LAST_ARGUMENTS_REGISTER_NUMBER) {
				ASMLRegister register = new ASMLRegister(ASMLRegister.FIRST_ARGUMENTS_REGISTER_NUMBER + i);
				assignments.add(new ASMLAssignment(register, getVariable(e.arguments.get(i))));
			} else
				throw new IllegalStateException("function '" + e.label.getName() + "' accepts " + e.arguments.size() + "arguments");
		}

		assignments.add(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, new ASMLCallLabel((ASMLLabel) e.label.accept(copier), List.of())));
		return assignments;
	}

	@Override
	public List<ASMLBase> visit(ASMLDivideFloat e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLDivideFloat(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLFloatLiteral e) {
		throw new IllegalStateException("register allocation of ASMLFloatLiteral");
	}

	@Override
	public List<ASMLBase> visit(ASMLIdentifier e) {
		return List.of(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, getVariable(e)));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfFloatEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfFloatEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfFloatGreaterOrEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfFloatGreaterOrEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfFloatLessOrEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfFloatLessOrEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfIdentifierOrImmediateEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfIdentifierOrImmediateEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfIdentifierOrImmediateGreaterOrEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfIdentifierOrImmediateGreaterOrEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIfIdentifierOrImmediateLessOrEqualElse e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.comp1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.comp2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		RegisterAllocationVisitor ifVisitor = new RegisterAllocationVisitor(offset);
		RegisterAllocationVisitor elseVisitor = new RegisterAllocationVisitor(offset);
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLIfIdentifierOrImmediateLessOrEqualElse(reg1, reg2, (ASMLBlock) e.ifBlock.accept(ifVisitor).get(0), (ASMLBlock) e.elseBlock.accept(elseVisitor).get(0)));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLIntegerLiteral e) {
		throw new IllegalStateException("register allocation of ASMLIntegerLiteral");
	}

	@Override
	public List<ASMLBase> visit(ASMLLabel e) {
		return new ArrayList<>(List.of(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, e)));
	}

	@Override
	public List<ASMLBase> visit(ASMLMemoryAssignment e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.base));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.offset));
		ASMLRegister reg3 = ASMLRegister.THIRD_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg3 = new ASMLAssignment(reg3, getVariable(e.assign));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLMemoryAssignment(reg1, reg2, reg3));
		return new ArrayList<>(List.of(loadReg1, loadReg2, loadReg3, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLMemoryRetrieval e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.base));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.offset));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLMemoryRetrieval(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLMultiplyFloat e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLMultiplyFloat(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLNegativeFloat e) {
		ASMLRegister register = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadRegister = new ASMLAssignment(register, getVariable(e.operator));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLNegativeFloat(register));
		return new ArrayList<>(List.of(loadRegister, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLNegativeIdentifier e) {
		ASMLRegister register = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadRegister = new ASMLAssignment(register, getVariable(e.operator));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLNegativeFloat(register));
		return new ArrayList<>(List.of(loadRegister, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLNewPlacement e) {
		ASMLRegister register = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadRegister = new ASMLAssignment(register, getVariable(e.operator));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLNewPlacement(register));
		return new ArrayList<>(List.of(loadRegister, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLNoOp e) {
		return new ArrayList<>(List.of(new ASMLAssignment(ASMLRegister.RESERVED_REGISTER, new ASMLNoOp())));
	}

	@Override
	public List<ASMLBase> visit(ASMLRegister e) {
		throw new IllegalStateException("register allocation of ASMLRegister");
	}

	@Override
	public List<ASMLBase> visit(ASMLPushRegister e) {
		throw new IllegalStateException("register allocation of ASMLPushRegister");
	}

	@Override
	public List<ASMLBase> visit(ASMLSubtractFloat e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLSubtractFloat(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLSubtractIdentifierOrImmediate e) {
		ASMLRegister reg1 = ASMLRegister.FIRST_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg1 = new ASMLAssignment(reg1, getVariable(e.op1));
		ASMLRegister reg2 = ASMLRegister.SECOND_GENERAL_PURPOSE_REGISTER;
		ASMLAssignment loadReg2 = new ASMLAssignment(reg2, getVariable(e.op2));
		ASMLRegister retReg = ASMLRegister.RESERVED_REGISTER;
		ASMLAssignment assign = new ASMLAssignment(retReg, new ASMLSubtractFloat(reg1, reg2));
		return new ArrayList<>(List.of(loadReg1, loadReg2, assign));
	}

	@Override
	public List<ASMLBase> visit(ASMLBlock e) {
		List<ASMLAssignment> newAssignments = new ArrayList<>(List.of());
		for (ASMLAssignment assignment : e.assignments)
			newAssignments.addAll(assignment.accept(this).stream().map(ass -> (ASMLAssignment) ass).collect(Collectors.toList()));
		newAssignments.addAll(e.result.accept(this).stream().map(ass -> (ASMLAssignment) ass).collect(Collectors.toList()));
		return new ArrayList<>(List.of(new ASMLBlock(newAssignments, ASMLRegister.RESERVED_REGISTER)));
	}

	@Override
	public List<ASMLBase> visit(ASMLProgram.ASMLFunction e) {
		List<ASMLIdentifier> args = new ArrayList<>(e.arguments);
		args.add(0, new ASMLIdentifier(ASMLLabel.SPECIAL));

		for (int i = 0; i < args.size(); i++) {
			int stackOffset = CALL_PARAMETERS_OFFSET + i * ASMLRegister.REGISTER_SIZE;
			variableMemoryOffsets.put(args.get(i).getName(), -stackOffset);
		}
		return new ArrayList<>(List.of(e.accept(copier)));
	}

	@Override
	public List<ASMLBase> visit(ASMLProgram e) {
		List<Pair<ASMLLabel, ASMLFloatLiteral>> copyGlobals = e.globals.stream().map(global -> new Pair<>((ASMLLabel) global.left.accept(copier), (ASMLFloatLiteral) global.right.accept(copier))).collect(Collectors.toList());
		List<Pair<ASMLProgram.ASMLFunction, ASMLBlock>> newFunctions = new ArrayList<>();
		for (Pair<ASMLProgram.ASMLFunction, ASMLBlock> function : e.functions) {
			RegisterAllocationVisitor functionVisitor = new RegisterAllocationVisitor();
			ASMLProgram.ASMLFunction newFunction = (ASMLProgram.ASMLFunction) function.left.accept(functionVisitor).get(0);
			ASMLBlock newBlock = (ASMLBlock) function.right.accept(functionVisitor).get(0);
			newFunctions.add(new Pair<>(newFunction, newBlock));
		}
		return new ArrayList<>(List.of(new ASMLProgram(copyGlobals, newFunctions, (ASMLBlock) e.main.accept(new RegisterAllocationVisitor()).get(0))));
	}
}
