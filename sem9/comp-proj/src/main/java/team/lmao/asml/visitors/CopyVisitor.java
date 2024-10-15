package team.lmao.asml.visitors;

import java.util.List;
import java.util.stream.Collectors;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBase;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;
import team.lmao.utils.Pair;

public class CopyVisitor implements Visitor<ASMLBase> {

	@Override
	public ASMLAddFloat visit(ASMLAddFloat e) {
		return new ASMLAddFloat((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifier) e.op2.accept(this));
	}

	@Override
	public ASMLAddIdentifierOrImmediate visit(ASMLAddIdentifierOrImmediate e) {
		return new ASMLAddIdentifierOrImmediate((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifierOrImmediate) e.op2.accept(this));
	}

	@Override
	public ASMLBase visit(ASMLAssignment e) {
		return new ASMLAssignment((ASMLIdentifier) e.left.accept(this), (ASMLExpression) e.right.accept(this));
	}

	@Override
	public ASMLCallClosureWithIdentifier visit(ASMLCallClosureWithIdentifier e) {
		return new ASMLCallClosureWithIdentifier((ASMLIdentifier) e.identifier.accept(this), e.arguments.stream().map(arg -> (ASMLIdentifier) arg.accept(this)).collect(Collectors.toList()));
	}

	@Override
	public ASMLCallLabel visit(ASMLCallLabel e) {
		return new ASMLCallLabel((ASMLLabel) e.label.accept(this), e.arguments.stream().map(arg -> (ASMLIdentifier) arg.accept(this)).collect(Collectors.toList()));
	}

	@Override
	public ASMLDivideFloat visit(ASMLDivideFloat e) {
		return new ASMLDivideFloat((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifier) e.op2.accept(this));
	}

	@Override
	public ASMLFloatLiteral visit(ASMLFloatLiteral e) {
		return new ASMLFloatLiteral(e.literal);
	}

	@Override
	public ASMLIdentifier visit(ASMLIdentifier e) {
		return new ASMLIdentifier(e.getName());
	}

	@Override
	public ASMLIfFloatEqualElse visit(ASMLIfFloatEqualElse e) {
		return new ASMLIfFloatEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifier) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIfFloatGreaterOrEqualElse visit(ASMLIfFloatGreaterOrEqualElse e) {
		return new ASMLIfFloatGreaterOrEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifier) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIfFloatLessOrEqualElse visit(ASMLIfFloatLessOrEqualElse e) {
		return new ASMLIfFloatLessOrEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifier) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIfIdentifierOrImmediateEqualElse visit(ASMLIfIdentifierOrImmediateEqualElse e) {
		return new ASMLIfIdentifierOrImmediateEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifierOrImmediate) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIfIdentifierOrImmediateGreaterOrEqualElse visit(ASMLIfIdentifierOrImmediateGreaterOrEqualElse e) {
		return new ASMLIfIdentifierOrImmediateGreaterOrEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifierOrImmediate) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIfIdentifierOrImmediateLessOrEqualElse visit(ASMLIfIdentifierOrImmediateLessOrEqualElse e) {
		return new ASMLIfIdentifierOrImmediateLessOrEqualElse((ASMLIdentifier) e.comp1.accept(this), (ASMLIdentifierOrImmediate) e.comp2.accept(this), (ASMLBlock) e.ifBlock.accept(this), (ASMLBlock) e.elseBlock.accept(this));
	}

	@Override
	public ASMLIntegerLiteral visit(ASMLIntegerLiteral e) {
		return new ASMLIntegerLiteral(e.literal);
	}

	@Override
	public ASMLLabel visit(ASMLLabel e) {
		return new ASMLLabel(e.getName());
	}

	@Override
	public ASMLMemoryAssignment visit(ASMLMemoryAssignment e) {
		return new ASMLMemoryAssignment((ASMLIdentifier) e.base.accept(this), (ASMLIdentifierOrImmediate) e.offset.accept(this), (ASMLIdentifier) e.assign.accept(this));
	}

	@Override
	public ASMLMemoryRetrieval visit(ASMLMemoryRetrieval e) {
		return new ASMLMemoryRetrieval((ASMLIdentifier) e.base.accept(this), (ASMLIdentifierOrImmediate) e.offset.accept(this));
	}

	@Override
	public ASMLMultiplyFloat visit(ASMLMultiplyFloat e) {
		return new ASMLMultiplyFloat((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifier) e.op2.accept(this));
	}

	@Override
	public ASMLNegativeFloat visit(ASMLNegativeFloat e) {
		return new ASMLNegativeFloat((ASMLIdentifier) e.operator.accept(this));
	}

	@Override
	public ASMLNegativeIdentifier visit(ASMLNegativeIdentifier e) {
		return new ASMLNegativeIdentifier((ASMLIdentifier) e.operator.accept(this));
	}

	@Override
	public ASMLNewPlacement visit(ASMLNewPlacement e) {
		return new ASMLNewPlacement((ASMLIdentifierOrImmediate) e.operator.accept(this));
	}

	@Override
	public ASMLNoOp visit(ASMLNoOp e) {
		return new ASMLNoOp();
	}

	@Override
	public ASMLRegister visit(ASMLRegister e) {
		return new ASMLRegister(e.getNumber());
	}

	@Override
	public ASMLPushRegister visit(ASMLPushRegister e) {
		return new ASMLPushRegister((ASMLRegister) e.register.accept(this));
	}

	@Override
	public ASMLSubtractFloat visit(ASMLSubtractFloat e) {
		return new ASMLSubtractFloat((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifier) e.op2.accept(this));
	}

	@Override
	public ASMLSubtractIdentifierOrImmediate visit(ASMLSubtractIdentifierOrImmediate e) {
		return new ASMLSubtractIdentifierOrImmediate((ASMLIdentifier) e.op1.accept(this), (ASMLIdentifierOrImmediate) e.op2.accept(this));
	}

	@Override
	public ASMLBlock visit(ASMLBlock e) {
		List<ASMLAssignment> copyAssignments = e.assignments.stream().map(ass -> (ASMLAssignment) ass.accept(this)).collect(Collectors.toList());
		return new ASMLBlock(copyAssignments, (ASMLExpression) e.result.accept(this));
	}

	@Override
	public ASMLBase visit(ASMLProgram.ASMLFunction e) {
		return new ASMLProgram.ASMLFunction((ASMLLabel) e.name.accept(this), e.arguments.stream().map(arg -> (ASMLIdentifier) arg.accept(this)).collect(Collectors.toList()));
	}

	@Override
	public ASMLProgram visit(ASMLProgram e) {
		List<Pair<ASMLLabel, ASMLFloatLiteral>> copyGlobals = e.globals.stream().map(global -> new Pair<>((ASMLLabel) global.left.accept(this), (ASMLFloatLiteral) global.right.accept(this))).collect(Collectors.toList());
		List<Pair<ASMLProgram.ASMLFunction, ASMLBlock>> copyFunctions = e.functions.stream().map(func -> new Pair<>((ASMLProgram.ASMLFunction) func.left.accept(this), (ASMLBlock) func.right.accept(this))).collect(Collectors.toList());
		return new ASMLProgram(copyGlobals, copyFunctions, (ASMLBlock) e.main.accept(this));
	}
}
