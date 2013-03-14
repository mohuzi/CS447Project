
$(TARGET)_3_65.out: $(TARGET).bc
	../get_callgraph.sh $< ; ../pipair <callgraph>$@ 

$(TARGET)_10_80.out: $(TARGET).bc
	../get_callgraph.sh $< ; ../pipair 10 80 <callgraph>$@

outputs: $(TARGET)_3_65.out $(TARGET)_10_80.out

