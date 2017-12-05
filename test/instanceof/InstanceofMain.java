public class InstanceofMain{
	public static void main(String[] args){
		Interface1 in1 = new Interface1Impl();
		Interface2 in2 = new Interface2Impl();		
		if(in1 instanceof Interface2){
			System.out.println("true");
		}else{
			
			System.out.println("false");
		}
		if(in2 instanceof Interface1){
			System.out.println("true");
		}else{
			
			System.out.println("false");
		}
		Interface1 in11 = (Interface1)in2;
		if(in11 instanceof Interface2){
			System.out.println("true");	
		}else{
			System.out.println("false");
		}
		in2 = new Interface2Impl2();
		if(in2 instanceof Interface1){
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		Interface2Impl2 il2 = new Interface2Impl2();
		if(il2 instanceof Interface2Impl){
			System.out.println("true");
		}else{
			System.out.println("false");
		}
		

	}
}
