require "ifj21"

function main()
    local a:integer = 5
    local b:string = "Good"
    local c:number

    c = (((#(b.."soup"))/2 + 15 + a - 1)*3)

    write(c, "\n")

end

main()

