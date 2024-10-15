package team.lmao.ast.visitors;

import java.util.*;

import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;
import team.lmao.ast.types.*;

public class KnormalizationVisitor extends CopyVisitor {
	private int tmpCount = 0;

	@Override
	public ASTExpression visit(ASTIntegerAdd e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTIntegerAdd innerIntA = new ASTIntegerAdd(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), innerIntA);
		return new ASTLet(tmp1.id, TypeInteger.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTIntegerSubtract e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTIntegerSubtract inner = new ASTIntegerSubtract(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), inner);
		return new ASTLet(tmp1.id, TypeInteger.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTFloatAdd e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTFloatAdd inner = new ASTFloatAdd(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), inner);
		return new ASTLet(tmp1.id, TypeFloat.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTFloatSubtract e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTFloatSubtract inner = new ASTFloatSubtract(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), inner);
		return new ASTLet(tmp1.id, TypeFloat.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTFloatMultiply e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTFloatMultiply inner = new ASTFloatMultiply(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), inner);
		return new ASTLet(tmp1.id, TypeFloat.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTFloatDivide e) {
		ASTVariable tmp1 = createTempVariable();
		ASTVariable tmp2 = createTempVariable();
		ASTFloatDivide inner = new ASTFloatDivide(tmp1, tmp2);
		ASTLet innerLet = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), inner);
		return new ASTLet(tmp1.id, TypeFloat.gen(), e.e1.accept(this), innerLet);
	}

	@Override
	public ASTExpression visit(ASTIntegerEqual e) {
		ASTVariable tmp1 = createTempVariable();
		ASTIntegerEqual inner = new ASTIntegerEqual(tmp1, e.e2);
		return new ASTLet(tmp1.id, TypeInteger.gen(), e.e1.accept(this), inner);
	}

	@Override
	public ASTExpression visit(ASTIntegerLessOrEqual e) {
		ASTVariable tmp1 = createTempVariable();
		ASTIntegerLessOrEqual inner = new ASTIntegerLessOrEqual(tmp1, e.e2);
		return new ASTLet(tmp1.id, TypeInteger.gen(), e.e1.accept(this), inner);
	}

	@Override
	public ASTExpression visit(ASTIf e) {
		ASTExpression innerLet2 = e.e2.accept(this);
		ASTExpression innerLet3 = e.e3.accept(this);
		if (e.e2.accept(this) instanceof ASTVariable) {
			ASTVariable tmp2 = createTempVariable();
			innerLet2 = new ASTLet(tmp2.id, Type.gen(), e.e2.accept(this), (ASTExpression) tmp2);
		}
		if (e.e3.accept(this) instanceof ASTVariable) {
			ASTVariable tmp3 = createTempVariable();
			innerLet3 = new ASTLet(tmp3.id, Type.gen(), e.e3.accept(this), (ASTExpression) tmp3);
		}
		ASTLet let = (ASTLet) e.e1.accept(this);
		ASTExpression innerLet1 = new ASTLet(let.id, Type.gen(), let.e1, new ASTIf(let.e2, innerLet2, innerLet3));
		return innerLet1;
	}


	@Override
	public ASTExpression visit(ASTApp e) {
		ASTExpression newLet = new ASTApp(e.e.accept(this), e.es);
		if (e.e.accept(this) instanceof ASTApp) {
			ASTVariable tmp1 = createTempVariable();
			newLet = new ASTLet(tmp1.id, Type.gen(), e.e.accept(this), new ASTApp(tmp1, e.es));
		} else {
			ASTVariable tmp = createTempVariable();
			ASTExpression res = e.e.accept(this);
			Integer count = 0;
			List<ASTExpression> newApp = new ArrayList<>();
			newApp.add(tmp);
			for (ASTExpression exp : e.es) {
				if (!(exp instanceof ASTInteger)) {
					res = exp.accept(this);
					count++;
				}
			}
			if (count != 0) {
				newLet = new ASTLet(tmp.id, Type.gen(), res, new ASTApp(e.e.accept(this), newApp));
			}

		}
		return newLet;
	}

	private ASTVariable createTempVariable() {
		String tmpName = "v" + tmpCount++;
		return new ASTVariable(new Id(tmpName));
	}
}
