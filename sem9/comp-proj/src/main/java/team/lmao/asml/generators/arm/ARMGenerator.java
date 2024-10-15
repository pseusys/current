package team.lmao.asml.generators.arm;

import java.util.ArrayList;
import java.util.List;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;
import team.lmao.asml.visitors.StringVisitor;
import team.lmao.asml.visitors.Visitor;
import team.lmao.exceptions.AssemblyGenerationException;
import team.lmao.utils.Pair;

public class ARMGenerator implements Visitor<List<String>> {
	private static final int ELSE_LABEL = 1;
	private static final int FI_LABEL = 2;

	private static final ASMLLabel HEAP_INIT = new ASMLLabel("_internal_init_heap");
	private static final ASMLLabel HEAP_ALLOC = new ASMLLabel("_internal_alloc_in_heap");
	private static final ASMLLabel GRAND_EXIT = new ASMLLabel("_min_caml_exit");

	String fileName;
	ARMRegister returnRegister;
	StringVisitor serializer = new StringVisitor();

	private ARMGenerator(String fileName, ARMRegister returnRegister) {
		this.fileName = fileName;
		this.returnRegister = returnRegister;
	}

	public ARMGenerator(String fileName) {
		this(fileName, ARMRegister.RETURN_REGISTER);
	}

	private ARMRegister validateOperand(ASMLIdentifierOrImmediate operand) {
		if (operand instanceof ASMLRegister) {
			return new ARMRegister((ASMLRegister) operand);
		} else {
			throw new AssemblyGenerationException("Operand " + operand.accept(serializer) + " is not a register!");
		}
	}

	private String validateRegisterOrImmediate(ASMLIdentifierOrImmediate operand) {
		if (operand instanceof ASMLRegister) {
			return ((ASMLRegister) operand).getName();
		} else if (operand instanceof ASMLIntegerLiteral) {
			return "#" + ((ASMLIntegerLiteral) operand).literal;
		} else {
			throw new AssemblyGenerationException("Operand " + operand.accept(serializer) + " is not a register or a literal!");
		}
	}

	@Override
	public List<String> visit(ASMLAddFloat e) {
		return List.of(ARMInstructions.moveFloat(validateOperand(e.op1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.op2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.addFloat(ARMRegister.RETURN_FLOAT_REGISTER, ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(returnRegister, ARMRegister.RETURN_FLOAT_REGISTER));
	}

	@Override
	public List<String> visit(ASMLAddIdentifierOrImmediate e) {
		return List.of(ARMInstructions.addInt(returnRegister, validateOperand(e.op1), validateRegisterOrImmediate(e.op2)));
	}

	@Override
	public List<String> visit(ASMLAssignment e) {
		return e.right.accept(new ARMGenerator(fileName, validateOperand(e.left)));
	}

	@Override
	public List<String> visit(ASMLCallClosureWithIdentifier e) {
		return List.of(ARMInstructions.storeRegistersCaller(), ARMInstructions.callRegister(validateOperand(e.identifier)), ARMInstructions.moveInt(ARMRegister.RETURN_REGISTER.toString(), returnRegister), ARMInstructions.restoreRegistersCaller());
	}

	@Override
	public List<String> visit(ASMLCallLabel e) {
		return List.of(ARMInstructions.storeRegistersCaller(), ARMInstructions.callLabel(e.label), ARMInstructions.moveInt(ARMRegister.RETURN_REGISTER.toString(), returnRegister), ARMInstructions.restoreRegistersCaller());
	}

	@Override
	public List<String> visit(ASMLDivideFloat e) {
		return List.of(ARMInstructions.moveFloat(validateOperand(e.op1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.op2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.divFloat(ARMRegister.RETURN_FLOAT_REGISTER, ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(returnRegister, ARMRegister.RETURN_FLOAT_REGISTER));
	}

	@Override
	public List<String> visit(ASMLFloatLiteral e) {
		return List.of(ARMInstructions.floatLiteral(e));
	}

	@Override
	public List<String> visit(ASMLIdentifier e) {
		return List.of(e.getName());
	}

	@Override
	public List<String> visit(ASMLIfFloatEqualElse e) {
		List<String> result = new ArrayList<>();
		result.addAll(List.of(ARMInstructions.moveFloat(validateOperand(e.comp1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.comp2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.compareFloat(ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloatFlagsToFlagRegister(), ARMInstructions.branchForwardToLocalLabelNotEqual(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(this));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(this));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIfFloatGreaterOrEqualElse e) {
		List<String> result = new ArrayList<>();
		ARMGenerator generator = new ARMGenerator(fileName, ARMRegister.RESERVED_REGISTER);
		result.addAll(List.of(ARMInstructions.moveFloat(validateOperand(e.comp1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.comp2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.compareFloat(ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloatFlagsToFlagRegister(), ARMInstructions.branchForwardToLocalLabelLessThan(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(generator));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(generator));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIfFloatLessOrEqualElse e) {
		List<String> result = new ArrayList<>();
		ARMGenerator generator = new ARMGenerator(fileName, ARMRegister.RESERVED_REGISTER);
		result.addAll(List.of(ARMInstructions.moveFloat(validateOperand(e.comp1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.comp2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.compareFloat(ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloatFlagsToFlagRegister(), ARMInstructions.branchForwardToLocalLabelGreaterThan(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(generator));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(generator));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIfIdentifierOrImmediateEqualElse e) {
		List<String> result = new ArrayList<>();
		ARMGenerator generator = new ARMGenerator(fileName, ARMRegister.RESERVED_REGISTER);
		result.addAll(List.of(ARMInstructions.compareInt(validateOperand(e.comp1), validateOperand(e.comp2)), ARMInstructions.branchForwardToLocalLabelNotEqual(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(generator));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(generator));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIfIdentifierOrImmediateGreaterOrEqualElse e) {
		List<String> result = new ArrayList<>();
		ARMGenerator generator = new ARMGenerator(fileName, ARMRegister.RESERVED_REGISTER);
		result.addAll(List.of(ARMInstructions.compareInt(validateOperand(e.comp1), validateOperand(e.comp2)), ARMInstructions.branchForwardToLocalLabelLessThan(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(generator));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(generator));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIfIdentifierOrImmediateLessOrEqualElse e) {
		List<String> result = new ArrayList<>();
		ARMGenerator generator = new ARMGenerator(fileName, ARMRegister.RESERVED_REGISTER);
		result.addAll(List.of(ARMInstructions.compareInt(validateOperand(e.comp1), validateOperand(e.comp2)), ARMInstructions.branchForwardToLocalLabelGreaterThan(ELSE_LABEL)));
		result.addAll(e.ifBlock.accept(generator));
		result.addAll(List.of(ARMInstructions.branchForwardToLocalLabelAlways(FI_LABEL), ARMInstructions.createLocalNumericLabel(ELSE_LABEL)));
		result.addAll(e.elseBlock.accept(generator));
		result.add(ARMInstructions.createLocalNumericLabel(FI_LABEL));
		return result;
	}

	@Override
	public List<String> visit(ASMLIntegerLiteral e) {
		return List.of(ARMInstructions.moveInt(validateRegisterOrImmediate(e), returnRegister));
	}

	@Override
	public List<String> visit(ASMLLabel e) {
		return List.of(ARMInstructions.loadLabel(returnRegister, e));
	}

	@Override
	public List<String> visit(ASMLMemoryAssignment e) {
		return List.of(ARMInstructions.storeRegister(validateOperand(e.assign), validateOperand(e.base), validateRegisterOrImmediate(e.offset)));
	}

	@Override
	public List<String> visit(ASMLMemoryRetrieval e) {
		return List.of(ARMInstructions.loadRegister(returnRegister, validateOperand(e.base), validateRegisterOrImmediate(e.offset)));
	}

	@Override
	public List<String> visit(ASMLMultiplyFloat e) {
		return List.of(ARMInstructions.moveFloat(validateOperand(e.op1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.op2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.multFloat(ARMRegister.RETURN_FLOAT_REGISTER, ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(returnRegister, ARMRegister.RETURN_FLOAT_REGISTER));
	}

	@Override
	public List<String> visit(ASMLNegativeFloat e) {
		return List.of(ARMInstructions.moveFloat(validateOperand(e.operator), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.negFloat(ARMRegister.RETURN_FLOAT_REGISTER, ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(returnRegister, ARMRegister.RETURN_FLOAT_REGISTER));
	}

	@Override
	public List<String> visit(ASMLNegativeIdentifier e) {
		return List.of(ARMInstructions.negInt(returnRegister, validateOperand(e.operator)));
	}

	@Override
	public List<String> visit(ASMLNewPlacement e) {
		ArrayList<String> result = new ArrayList<>();
		result.add(ARMInstructions.moveInt(validateRegisterOrImmediate(e.operator), ARMRegister.RETURN_REGISTER));
		result.addAll((new ASMLCallLabel(HEAP_ALLOC, List.of())).accept(this));
		return result;
	}

	@Override
	public List<String> visit(ASMLNoOp e) {
		return List.of(ARMInstructions.nop());
	}

	@Override
	public List<String> visit(ASMLRegister e) {
		return List.of(ARMInstructions.moveInt(e.getName(), ARMRegister.RETURN_REGISTER));
	}

	@Override
	public List<String> visit(ASMLPushRegister e) {
		return List.of(ARMInstructions.pushRegister(new ARMRegister(e.register)));
	}

	@Override
	public List<String> visit(ASMLSubtractFloat e) {
		return List.of(ARMInstructions.moveFloat(validateOperand(e.op1), ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(validateOperand(e.op2), ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.subFloat(ARMRegister.RETURN_FLOAT_REGISTER, ARMRegister.FIRST_GENERAL_PURPOSE_FLOAT_REGISTER, ARMRegister.SECOND_GENERAL_PURPOSE_FLOAT_REGISTER), ARMInstructions.moveFloat(returnRegister, ARMRegister.RETURN_FLOAT_REGISTER));
	}

	@Override
	public List<String> visit(ASMLSubtractIdentifierOrImmediate e) {
		return List.of(ARMInstructions.subInt(returnRegister, validateOperand(e.op1), validateRegisterOrImmediate(e.op2)));
	}

	@Override
	public List<String> visit(ASMLBlock e) {
		List<String> result = new ArrayList<>();
		for (ASMLAssignment assignment : e.assignments)
			result.addAll(assignment.accept(this));
		result.addAll((new ASMLAssignment(returnRegister.toASMLRegister(), e.result)).accept(this));
		return result;
	}

	@Override
	public List<String> visit(ASMLProgram.ASMLFunction e) {
		return e.name.accept(this);
	}

	private List<String> visitFunction(Pair<ASMLProgram.ASMLFunction, ASMLBlock> function, boolean main) {
		ArrayList<String> result = new ArrayList<>(List.of(ARMInstructions.createLabel(function.left.name)));
		if (!main)
			result.add(ARMInstructions.storeRegistersCallee());
		result.addAll(List.of(ARMInstructions.subInt(ARMRegister.STACK_REGISTER, ARMRegister.STACK_REGISTER, "#4"), ARMInstructions.moveInt(ARMRegister.STACK_REGISTER.toString(), ARMRegister.FRAME_REGISTER)));
		if (main)
			result.addAll((new ASMLCallLabel(HEAP_INIT, List.of())).accept(this));
		result.addAll(function.right.accept(new ARMGenerator(fileName, ARMRegister.RETURN_REGISTER)));
		result.addAll(List.of(ARMInstructions.moveInt(ARMRegister.FRAME_REGISTER.toString(), ARMRegister.STACK_REGISTER), ARMInstructions.addInt(ARMRegister.STACK_REGISTER, ARMRegister.STACK_REGISTER, "#4")));
		if (!main)
			result.addAll(List.of(ARMInstructions.restoreRegistersCallee(), ARMInstructions.callRegister(ARMRegister.RETURN_ADDRESS_REGISTER)));
		else
			result.add(ARMInstructions.callLabel(GRAND_EXIT));
		return result;
	}

	@Override
	public List<String> visit(ASMLProgram e) {
		ArrayList<String> result = new ArrayList<>(ARMInstructions.createPrefix(fileName));
		if (!e.globals.isEmpty())
			result.addAll(List.of("", "@ LITERALS:"));
		for (Pair<ASMLLabel, ASMLFloatLiteral> global : e.globals)
			result.addAll(List.of(ARMInstructions.createLabel(global.left), global.right.accept(this).get(0)));
		if (!e.functions.isEmpty())
			result.addAll(List.of("", "", "@ FUNCTIONS:"));
		for (Pair<ASMLProgram.ASMLFunction, ASMLBlock> function : e.functions)
			result.addAll(visitFunction(function, false));
		result.addAll(List.of("", "", "@ MAIN:"));
		result.addAll(visitFunction(new Pair<>(new ASMLProgram.ASMLFunction(ASMLLabel.MAIN_NAME, List.of()), e.main), true));
		result.addAll(List.of("", "", "@ COMPILER:", ARMInstructions.createPostfix(), ""));
		return result;
	}
}
