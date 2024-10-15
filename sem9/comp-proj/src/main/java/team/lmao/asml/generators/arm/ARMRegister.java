package team.lmao.asml.generators.arm;

import team.lmao.asml.expressions.ASMLRegister;
import team.lmao.exceptions.RegisterAllocationException;

public class ARMRegister {
	private static final int LAST_FLOAT_REGISTER = 4;

	public static final ARMRegister STACK_REGISTER = new ARMRegister("R13");
	public static final ARMRegister RETURN_ADDRESS_REGISTER = new ARMRegister("R14");
	public static final ARMRegister RESERVED_REGISTER = new ARMRegister(ASMLRegister.RESERVED_REGISTER);
	public static final ARMRegister FRAME_REGISTER = new ARMRegister(ASMLRegister.STACK_FRAME_REGISTER);
	public static final ARMRegister RETURN_REGISTER = new ARMRegister(ASMLRegister.RETURN_REGISTER);
	public static final ARMRegister RETURN_FLOAT_REGISTER = new ARMRegister(0);
	public static final ARMRegister FIRST_GENERAL_PURPOSE_FLOAT_REGISTER = new ARMRegister(1);
	public static final ARMRegister SECOND_GENERAL_PURPOSE_FLOAT_REGISTER = new ARMRegister(2);

	private final String name;

	private ARMRegister(String name) {
		this.name = name;
	}

	public ARMRegister(ASMLRegister register) {
		this.name = register.getName();
	}

	public ARMRegister(int floatRegister) {
		String name = "S" + floatRegister;
		if (floatRegister > LAST_FLOAT_REGISTER)
			throw new RegisterAllocationException(name);
		this.name = name;
	}

	public ASMLRegister toASMLRegister() {
		ASMLRegister register = new ASMLRegister(ASMLRegister.LAST_GENERAL_PURPOSE_REGISTER.getNumber());
		register.setName(name);
		return register;
	}

	@Override
	public String toString() {
		return name;
	}
}
