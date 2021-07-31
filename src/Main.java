import java.io.IOException;

public class Main {

	public static void main(String[] args) {
		String sourceFile = "test/helloworld.hr";
		String destFile = "test/helloworld.c";
		
		Compiler comp = new Compiler(sourceFile);
		try {
			comp.start(destFile);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
