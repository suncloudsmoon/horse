import java.io.IOException;

public class Main {
	
	public static String testOrNot = "test/";

	public static void main(String[] args) {
		String sourceFile = "test/Compiler.hr", destFile = "test/helloworld.c";
		if (args.length > 0) {
			sourceFile = args[0];
			destFile = args[1];
			testOrNot = "";
		}
		
		Compiler comp = new Compiler(sourceFile);
		try {
			// TODO: replace this with a logger
			System.out.println("Program Started!");
			comp.start(destFile);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
