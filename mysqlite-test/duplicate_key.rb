describe "database" do
    def run_script(commands)
        raw_output = nil
        IO.popen("../build/mysqlite duplicate", "r+") do | pipe |
            commands.each do | command |
                pipe.puts command
            end
            pipe.close_write
            raw_output = pipe.gets(nil)
        end
        raw_output.split("\n");
    end

   it 'print an error message if there is a duplicate id' do
        script = [
            "insert 1 user1 person1@example.com",
            "insert 1 user1 person1@example.com",
            "select",
            ".exit",
        ]
        result = run_script(script)
        expect(result).to match_array([
            "db > Executed.",
            "db > Error: Duplicate key.",
            "db > (1, user1, person1@example.com)",
            "Executed.",
            "db > ",
        ])
   end

end