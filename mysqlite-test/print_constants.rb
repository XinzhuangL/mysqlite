describe "database" do
    def run_script(commands)
        raw_output = nil
        IO.popen("../build/mysqlite print", "r+") do | pipe |
            commands.each do | command |
                pipe.puts command
            end
            pipe.close_write
            raw_output = pipe.gets(nil)
        end
        raw_output.split("\n");
    end

    it 'prints constants' do
                script = [
                    ".constants",
                    ".exit",
                ]
                result = run_script(script)
                expect(result).to match_array([
                    "db > Constants: ",
                    "ROW_SIZE: 293",
                    "COMMON_NODE_HEADER_SIZE: 6",
                    "LEAF_NODE_HEADER_SIZE: 10",
                    "LEAF_NODE_CELL_SIZE: 297",
                    "LEAF_NODE_SPACE_FOR_CELLS: 4086",
                    "LEAF_NODE_MAX_CELLS: 13",
                    "db > ",
                ])
    end

    # insert 3 user3 person3@example.com
    # insert 1 user1 person1@example.com
    # insert 2 user2 person2@example.com

    it 'allows printing out the structure of a one-node btree' do
                script = [3, 1, 2].map do |i|
                    "insert #{i} user#{i} person#{i}@example.com"
                end
                script << ".btree"
                script << ".exit"
                result = run_script(script)

                expect(result).to match_array([
                    "db > Executed.",
                    "db > Executed.",
                    "db > Executed.",
                    "db > Tree:",
                    "leaf (size 3)",
                    " - 0 : 3",
                    " - 1 : 1",
                    " - 2 : 2",
                    "db > "
                ])
    end
end