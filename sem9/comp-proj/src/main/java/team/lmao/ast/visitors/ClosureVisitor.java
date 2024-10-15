package team.lmao.ast.visitors;

import java.util.*;

import team.lmao.ast.Id;
import team.lmao.ast.expressions.*;
import team.lmao.ast.types.Type;

public class ClosureVisitor extends CopyVisitor {
	public static List<ASTFunctionDefine> funcList = new ArrayList<>();
	List<Id> labels = new ArrayList<>();

	public static List<ASTFunctionDefine> returnListFunctions() {
		return funcList;
	}

	@Override
	public ASTExpression visit(ASTLetRec e) {
		if (e.fd.e instanceof ASTLetRec) {

			ASTLetRec inner = (ASTLetRec) e.fd.e;
			inner.fd.label = "_" + inner.fd.id;
			funcList.add(inner.fd);

			ASTClosure clos = new ASTClosure(inner.fd.id, e.fd.args);
			clos.label = "_" + clos.id.toString();
			Id idd = Id.gen();
			ASTLet newLet = new ASTLet(idd, Type.gen(), clos, new ASTVariable(idd));
			ASTFunctionDefine func = new ASTFunctionDefine(e.fd.id, e.fd.type, e.fd.args, newLet);
			func.label = "_" + e.fd.id.toString();
			funcList.add(func);

		} else if (e.fd.e instanceof ASTApp) {
			ASTApp app = (ASTApp) e.fd.e;
			ASTVariable var = (ASTVariable) app.e;
			List<ASTVariable> vv = getVariables(app.es);
			List<Id> args = new ArrayList<>();
			for (ASTVariable a : vv) {
				args.add(a.id);
				System.out.println(a.id.toString());
			}
			ASTCallClosure clos = new ASTCallClosure(var.id, args);
			ASTFunctionDefine func = new ASTFunctionDefine(e.fd.id, e.fd.type, e.fd.args, clos);
			func.label = "_" + func.id;
			funcList.add(func);
		} else {
			e.fd.label = "_" + e.fd.id;
			funcList.add(e.fd);
		}
		return e.e.accept(this);
	}

	public Void PrintFunctions() {
		System.out.println("Functions definitions : \n");
		for (ASTFunctionDefine e : funcList) {

			System.out.println("label : " + e.label);
			System.out.println("params : ");
			for (Id i : e.args) {
				System.out.println(i.toString() + " ");
			}
			System.out.println("code : ");
			System.out.println(e.e.accept(new StringVisitor()));
			System.out.println();
		}
		return null;
	}

	@Override
	public ASTExpression visit(ASTApp e) {
		ASTExpression res = new ASTApp(e.e.accept(this), e.es);
		List<ASTVariable> vars = getVariables(e.es);
		List<Id> args = new ArrayList<>();
		for (ASTVariable a : vars) {
			args.add(a.id);
		}
		ASTVariable lab = (ASTVariable) e.e.accept(this);
		PrintFunctions();
		if (labels.contains(lab.id)) {
			res = new ASTCallClosure(lab.id, args);
		}
		for (ASTFunctionDefine exp : funcList) {
			if (lab.id.toString().equals(exp.id.toString())) {
				ASTFunctionDefine callFunc = new ASTFunctionDefine(exp.id, exp.type, args, exp.e);
				callFunc.label = exp.label;
				res = callFunc;
			}
		}
		return res;
	}

	@Override
	public ASTExpression visit(ASTLet e) {
		if (e.e1.accept(this) instanceof ASTFunctionDefine) {
			labels.add(e.id);
		}
		return new ASTLet(e.id, e.t, e.e1.accept(this), e.e2.accept(this));
	}

	public <T> List<ASTVariable> getVariables(List<T> list) {
		List<ASTVariable> args = new ArrayList<>();
		for (T i : list) {
			if (i instanceof Id || i instanceof String) {
				ASTVariable arg = ASTVariable.get(i.toString());
				args.add(arg);
			} else if (i instanceof ASTInteger) {
				ASTInteger intExp = (ASTInteger) i;
				String iid = String.valueOf(intExp.i);
				ASTVariable arg = ASTVariable.get(iid);
				args.add(arg);
			} else {
				ASTVariable v1 = (ASTVariable) i;
				ASTVariable arg = ASTVariable.get(v1.id.toString());
				args.add(arg);
			}

		}
		return args;
	}

}
