qSim:
	@gcc -c ./src/qSim.c
	@mv qSim.o ./obj
	@gcc -o qSim ./obj/qSim.o
	@mv qSim ./bin

run:
	@echo ""
	@while read -r file; do echo ./qSim $$file && ./bin/qSim $$file; done <./TestCases/testcase1.txt
	@gnuplot gnuscript.gnu

clean:
	@rm ./output/plot.png
	@rm ./output/data.txt
	@rm ./bin/qSim
	@rm ./obj/qSim.o
