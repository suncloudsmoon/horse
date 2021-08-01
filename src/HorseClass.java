import java.util.LinkedList;
import java.util.List;

public class HorseClass {
	
	private String name;
	
	// Headers, etc.
	private String header;
	
	private List<String> variableIndexList;
	private List<String> compiledVariables;
	private List<String> compiledLines;
	
	public HorseClass(String name, String header) {
		this.name = name;
		this.header = header;
		
		// Initializing new objects...
		variableIndexList = new LinkedList<String>();
		compiledLines = new LinkedList<String>();
		compiledVariables = new LinkedList<String>();
	}
	
	public void add(String compiledLine) {
		compiledLines.add(compiledLine);
	}
	
	/**
	 * Can also add function names (like C where you can treat functions like pointers)
	 * @param var
	 */
	public void addVariable(String var) {
		compiledVariables.add(var);
	}
	
	public void addVariableIndex(String varName) {
		variableIndexList.add(varName);
	}
	
	public String getPackage() {
		return header + "\n" + getAllAttributes() + "\n" + getAllLines();
	}
	
	private String getAllAttributes() {
		String fullTypedef = "typedef struct {\n";
		for (String var : compiledVariables) {
			fullTypedef += var + "\n";
			System.out.println("var: " + var + "\n");
		}
		fullTypedef += "\n} " + name + ";\n";
		return fullTypedef;
	}
	
	private String getAllLines() {
		String lines = "";
		for (String line : compiledLines) {
			lines += line + "\n";
		}
		return lines;
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}
}
