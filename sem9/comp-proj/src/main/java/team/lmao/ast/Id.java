package team.lmao.ast;

public class Id {
	private static int GLOBAL_STATE = -1;
	public String id;

	public Id(String id) {
		this.id = id;
	}

	@Override
	public String toString() {
		return id;
	}

	char var;

	public Id gen2() {

		if (!(this.id == null)) {
			var = this.id.charAt(0);
		}
		GLOBAL_STATE++;
		String NewId = String.format("%c%d", var, GLOBAL_STATE);
		return new Id(NewId);
	}

	@Override
	public boolean equals(Object o) {
		if (o == this) {
			return true;
		}
		if (!(o instanceof Id)) {
			return false;
		}
		Id cc = (Id) o;
		return cc.id.equals(this.id);
	}

	public static Id gen() {
		GLOBAL_STATE++;
		return new Id("v" + GLOBAL_STATE);
	}
}
