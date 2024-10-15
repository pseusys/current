package team.lmao.asml.expressions;

import team.lmao.asml.visitors.Visitor;
import team.lmao.exceptions.RegisterAllocationException;

public class ASMLRegister extends ASMLIdentifier {
	public static final int REGISTER_SIZE = 4;
	public static final int FIRST_ARGUMENTS_REGISTER_NUMBER = 0;
	public static final int LAST_ARGUMENTS_REGISTER_NUMBER = 3;
	public static final ASMLRegister RETURN_REGISTER = new ASMLRegister("R0");
	public static final ASMLRegister RESERVED_REGISTER = new ASMLRegister("R4");
	public static final ASMLRegister FIRST_GENERAL_PURPOSE_REGISTER = new ASMLRegister("R5");
	public static final ASMLRegister SECOND_GENERAL_PURPOSE_REGISTER = new ASMLRegister("R6");
	public static final ASMLRegister THIRD_GENERAL_PURPOSE_REGISTER = new ASMLRegister("R7");
	public static final ASMLRegister LAST_GENERAL_PURPOSE_REGISTER = new ASMLRegister("R10");
	public static final ASMLRegister STACK_FRAME_REGISTER = new ASMLRegister("R11");
	public static final ASMLRegister CLOSURE_REGISTER = new ASMLRegister("R12");

	private final boolean validateName;

	private ASMLRegister(String number) {
		super("R0");
		this.validateName = false;
		this.setName(number);
	}

	public ASMLRegister(int number) {
		super("R" + number);
		this.validateName = true;
	}

	@Override
	public <E> E accept(Visitor<E> v) {
		return v.visit(this);
	}

	public int getNumber() {
		return Integer.parseInt(getName().substring(1));
	}

	@Override
	protected String validateName(String name) {
		if (!validateName)
			return name;
		String registerNumber = name.substring(1);
		try {
			int number = Integer.parseInt(registerNumber);
			if (number > 0 && number != 4 && number <= 10)
				throw new RegisterAllocationException(name);
			return Character.toUpperCase(name.charAt(0)) + registerNumber;
		} catch (NumberFormatException nfe) {
			throw new RegisterAllocationException(name);
		}
	}
}
