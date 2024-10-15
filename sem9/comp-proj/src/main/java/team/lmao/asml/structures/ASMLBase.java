package team.lmao.asml.structures;

import team.lmao.asml.visitors.Visitor;

public interface ASMLBase {
	<E> E accept(Visitor<E> v);
}
