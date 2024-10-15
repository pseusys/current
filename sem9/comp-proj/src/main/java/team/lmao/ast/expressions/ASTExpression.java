package team.lmao.ast.expressions;

import team.lmao.ast.visitors.Visitor;

public interface ASTExpression {
	<E> E accept(Visitor<E> v);
}
