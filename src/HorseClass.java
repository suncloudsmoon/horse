import java.util.LinkedList;
import java.util.List;

public class HorseClass {
	
	private String name;
	
	// Headers, etc.
	private String header;
	private String variables;
	private List<String> compiled;
	
	public HorseClass(String name, String header) {
		this.name = name;
		this.header = header;
		
		// Initializing new objects...
		compiled = new LinkedList<String>();
	}
	
	public void add(String compiledLine) {
		compiled.add(compiledLine);
	}
	
	public String getPackage() {
		String newStr = header + "\n" + getAllAttributes() + "\n";
		for (String line : compiled) {
			newStr += line;
		}
		return newStr;
	}
	
	private String getAllAttributes() {
		return "typedef struct {\n" + variables + "\n} " + name + ";\n";
	}

	/**
	 * @return the name
	 */
	public String getName() {
		return name;
	}
}
