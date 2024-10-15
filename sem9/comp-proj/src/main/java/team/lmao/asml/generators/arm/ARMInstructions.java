package team.lmao.asml.generators.arm;

import java.util.List;

import team.lmao.asml.expressions.ASMLFloatLiteral;
import team.lmao.asml.expressions.ASMLIntegerLiteral;
import team.lmao.asml.expressions.ASMLLabel;
import team.lmao.utils.ConsoleApp;

public class ARMInstructions {
	public static List<String> createPrefix(String fileName) {
		return List.of(".file \"" + fileName + "\"", ".text", ".global " + ASMLLabel.MAIN_NAME.getName(), ".arm");
	}

	public static String createPostfix() {
		return ".ident \"LMAO: " + ConsoleApp.getVersion() + "\"";
	}

	public static String nop() {
		return "nop";
	}

	public static String createLabel(ASMLLabel label) {
		return label.getName() + ":";
	}

	public static String intLiteral(ASMLIntegerLiteral literal) {
		return ".word " + literal.literal;
	}

	public static String floatLiteral(ASMLFloatLiteral literal) {
		return ".float " + literal.literal;
	}

	public static String storeRegister(ARMRegister register, ARMRegister address, String offset) {
		return "str " + register + ", [" + address + ", " + offset + "]";
	}

	public static String loadRegister(ARMRegister register, ARMRegister address, String offset) {
		return "ldr " + register + ", [" + address + ", " + offset + "]";
	}

	public static String loadLabel(ARMRegister register, ASMLLabel address) {
		return "ldr " + register + ", =" + address.getName();
	}

	public static String moveInt(String from, ARMRegister to) {
		return "mov " + to + ", " + from;
	}

	public static String negInt(ARMRegister from, ARMRegister to) {
		return "mvn " + to + ", " + from;
	}

	public static String moveFloat(ARMRegister from, ARMRegister to) {
		return "vmov " + to + ", " + from;
	}

	public static String negFloat(ARMRegister from, ARMRegister to) {
		return "mnf " + to + ", " + from;
	}

	public static String addInt(ARMRegister destination, ARMRegister op1, String op2) {
		return "add " + destination + ", " + op1 + ", " + op2;
	}

	public static String subInt(ARMRegister destination, ARMRegister op1, String op2) {
		return "sub " + destination + ", " + op1 + ", " + op2;
	}

	public static String addFloat(ARMRegister destination, ARMRegister op1, ARMRegister op2) {
		return "vadd.f32 " + destination + ", " + op1 + ", " + op2;
	}

	public static String subFloat(ARMRegister destination, ARMRegister op1, ARMRegister op2) {
		return "vsub.f32 " + destination + ", " + op1 + ", " + op2;
	}

	public static String multFloat(ARMRegister destination, ARMRegister op1, ARMRegister op2) {
		return "vmul.f32 " + destination + ", " + op1 + ", " + op2;
	}

	public static String divFloat(ARMRegister destination, ARMRegister op1, ARMRegister op2) {
		return "vdiv.f32 " + destination + ", " + op1 + ", " + op2;
	}

	public static String compareFloat(ARMRegister op1, ARMRegister op2) {
		return "vcmp.f32 " + op1 + ", " + op2;
	}

	public static String moveFloatFlagsToFlagRegister() {
		return "vmrs APSR_nzcv, FPSCR";
	}

	public static String compareInt(ARMRegister op1, ARMRegister op2) {
		return "cmp " + op1 + ", " + op2;
	}

	public static String createLocalNumericLabel(int label) {
		return label + ":";
	}

	public static String branchForwardToLocalLabelAlways(int label) {
		return "bal " + label + "f";
	}

	public static String branchForwardToLocalLabelNotEqual(int label) {
		return "bne " + label + "f";
	}

	public static String branchForwardToLocalLabelLessThan(int label) {
		return "blt " + label + "f";
	}

	public static String branchForwardToLocalLabelGreaterThan(int label) {
		return "bgt " + label + "f";
	}

	public static String callLabel(ASMLLabel label) {
		return "bl " + label.getName();
	}

	public static String callRegister(ARMRegister register) {
		return "bx " + register;
	}

	private static String storeRegisters(String registers) {
		return "push {" + registers + "}";
	}

	public static String pushRegister(ARMRegister register) {
		return storeRegisters(register.toString());
	}

	public static String storeRegistersCaller() {
		return storeRegisters("R0-R3, R12, R14");
	}

	public static String storeRegistersCallee() {
		return storeRegisters("R4-R11");
	}

	private static String restoreRegisters(String registers) {
		return "pop {" + registers + "}";
	}

	public static String restoreRegistersCaller() {
		return restoreRegisters("R0-R3, R12, R14");
	}

	public static String restoreRegistersCallee() {
		return restoreRegisters("R4-R11");
	}
}
