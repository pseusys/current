package team.lmao.asml.visitors;

import team.lmao.asml.expressions.*;
import team.lmao.asml.structures.ASMLBlock;
import team.lmao.asml.structures.ASMLProgram;

public interface Visitor<E> {
	E visit(ASMLAddFloat e);

	E visit(ASMLAddIdentifierOrImmediate e);

	E visit(ASMLAssignment e);

	E visit(ASMLCallClosureWithIdentifier e);

	E visit(ASMLCallLabel e);

	E visit(ASMLDivideFloat e);

	E visit(ASMLFloatLiteral e);

	E visit(ASMLIdentifier e);

	E visit(ASMLIfFloatEqualElse e);

	E visit(ASMLIfFloatGreaterOrEqualElse e);

	E visit(ASMLIfFloatLessOrEqualElse e);

	E visit(ASMLIfIdentifierOrImmediateEqualElse e);

	E visit(ASMLIfIdentifierOrImmediateGreaterOrEqualElse e);

	E visit(ASMLIfIdentifierOrImmediateLessOrEqualElse e);

	E visit(ASMLIntegerLiteral e);

	E visit(ASMLLabel e);

	E visit(ASMLMemoryAssignment e);

	E visit(ASMLMemoryRetrieval e);

	E visit(ASMLMultiplyFloat e);

	E visit(ASMLNegativeFloat e);

	E visit(ASMLNegativeIdentifier e);

	E visit(ASMLNewPlacement e);

	E visit(ASMLNoOp e);

	E visit(ASMLPushRegister e);

	E visit(ASMLRegister e);

	E visit(ASMLSubtractFloat e);

	E visit(ASMLSubtractIdentifierOrImmediate e);

	E visit(ASMLBlock e);

	E visit(ASMLProgram.ASMLFunction e);

	E visit(ASMLProgram e);
}
