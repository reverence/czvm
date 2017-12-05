public class TestInstanceof{
	public static void main(String[] args){
		Sub sub = new Sub();
		if(sub instanceof Super){
			System.out.println("true");
		}
	}
}

class Super{

}

class Sub extends Super{

}
