package team.lmao.asml.structures;

import java.util.LinkedList;
import java.util.List;

import team.lmao.asml.expressions.ASMLAssignment;
import team.lmao.asml.expressions.ASMLExpression;
import team.lmao.asml.visitors.Visitor;

public class ASMLBlock implements ASMLBase {
	public List<ASMLAssignment> assignments;

	public ASMLExpression result;

	public ASMLBlock(List<ASMLAssignment> assignments, ASMLExpression result) {
		this.assignments = assignments;
		this.result = result;
	}

	public ASMLBlock(ASMLExpression result) {
		this(new LinkedList<>(), result);
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}
}
