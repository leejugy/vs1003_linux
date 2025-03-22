all : $(SRC)
	$(CC) -o $(OBJECT) $^ $(CPPFLAGS) $(LDFLAGS) $(CFLAGS)

clean:
	rm -rf $(OBJECT) *.o