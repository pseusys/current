package team.lmao.ast;

import java.io.Reader;

import team.lmao.ast.expressions.ASTExpression;
import team.lmao.ast.visitors.*;
import team.lmao.exceptions.SourceParseException;
import team.lmao.utils.Logger;

public class AST {
	final public ASTExpression root;

	private AST(ASTExpression root) {
		this.root = root;
	}

	public static AST parse(Reader reader, Logger logger) {
		try {
			logger.infoln("Source MinCaml file parsing...");
			ASTParser p = new ASTParser(reader);
			ASTExpression root = (ASTExpression) p.parse().value;
			if (root == null)
				throw new RuntimeException("Root ASTExpression is null!");
			AST tree = new AST(root);
			logger.successln("MinCaml file converted to AST!");
			logger.debugln("AST representation: " + tree.serialize());
			return tree;
		} catch (Exception e) {
			throw new SourceParseException("AST parsing exception", e);
		}
	}

	public String serialize() {
		return "\n" + accept(new StringVisitor());
	}

	public AST copy() {
		return new AST(root.accept(new CopyVisitor()));
	}

	public AST k_normalize() {
		return new AST(root.accept(new KnormalizationVisitor()));
	}

	public AST let_reduce() {
		return new AST(root.accept(new LetReductionVisitor()));
	}

	public AST closure_convert() {
		return new AST(root.accept(new ClosureVisitor()));
	}

	public <E> E accept(Visitor<E> visitor) {
		return root.accept(visitor);
	}
}
