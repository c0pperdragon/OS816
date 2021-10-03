var identifiers;
var finalpass;
var pc;
var output;

function parse(line)
{
    // expand quoted strings to sequence of numbers
    for (;;)
    {
        var qm1 = line.indexOf('"');
        if (qm1<0) { break; }
        var qm2 = line.indexOf('"', qm1+1);
        if (qm2<0) { break; }
        var expanded = [];
        for (i=qm1+1; i<qm2; i++) { expanded.push(line.charCodeAt(i)); }
        line = line.substring(0,qm1) + expanded.join(",") + line.substring(qm2+1);
    }    
    line = line.toUpperCase();               // do all upper case
    var comment = line.indexOf(";");         // strip comments
    if (comment>=0) { line = line.substring(0,comment); }

    var parts = line.trimRight().split(/\s+/);    // break at blanks
    while (parts.length<3) { parts.push(null); }  // fill up array to 3 
    while (parts.length>3) { parts.pop(); }       // remove excess 
    if (parts[0]=='') { parts[0]=null; }          // convert possibly empty token to null
    
    return parts;
}

function preprocess(code)
{
    var tokenlines = code.split("\n").map(parse);
    return tokenlines.map(tl => x(tl[0]).padEnd(15)+"\t"+x(tl[1])+"\t"+x(tl[2])).join("\n");
    function x(t) { return t?t:""; }
}

function assemble(code)
{
    output = []
    identifiers = {};
    var linenr;
    try 
    {
        var tokenlines = code.split("\n").map(parse);        

        finalpass = false;        
        pc = 0;
        for (linenr=0; linenr<tokenlines.length; linenr++)
        {
            process(tokenlines[linenr]); 
        }    
        
        finalpass = true;
        pc = 0;
        for (linenr=0; linenr<tokenlines.length; linenr++)
        { 
            output.push(pc.toString(16).toUpperCase().padStart(6,"0"));
            output.push(" ");
            process(tokenlines[linenr]); 
            output.push("\n");        
        }
    }
    catch (message) 
    {
        if ((typeof message) !== "string") { throw message; }
        return "Assembly error in line "+(linenr+1)+":\n" + message;
    }
    return output.join("");
}


function process(tokens)
{
    var label = tokens[0];
    var instr = tokens[1];
    var operand = tokens[2];
    if (!finalpass && label) 
    {
        if (identifiers.hasOwnProperty(label)) { throw "Redefining '"+label+"'"; }
        identifiers[label] = (instr==="SET") ? compute(operand) : pc;
    }
    if (instr===null || (instr==="SET" && label)) { return; }
    if (instr==="ORG") { pc = compute(operand); return; }
    if (instr==="DB") 
    {
        for (b of operand.split(",")) 
        {
            value = compute(b, 0);
            if (value<0 || value>255) { throw "Data outside 8 bit range: "+value; }
            emit (value);
        }            
        return;
    }
    if (instr==="DW") 
    {
        for (b of operand.split(",")) 
        {
            value = compute(b, 0);
            if (value<0 || value>65535) { throw "Data outside 16 bit range: "+value; }
            emit(value & 0xff);
            emit((value>>8) & 0xff);
        }      
        return;
    }
    if (instr==="ST8") 
    {
        emit(0xE2);  // switch to 8 bit accu
        emit(0x02);
        process ([null, "STA", operand]);
        emit(0xC2);  // back to normal
        emit(0x02);
        return;
    }
    if (!opcodetable.hasOwnProperty(instr)) { throw "Unknown instruction '"+instr+"'"; }

    var mode = amode.implicit;  
    var value = 0;
    if (operand)
    {
        if (operand.startsWith(">")) 
        {
            if (operand.endsWith(",X"))
            {
                mode = amode.long_x;
                value = compute(operand.substring(1, operand.length-2), 0);
            }
            else
            {
                mode = amode.long;
                value = compute(operand.substring(1), 0);
            }
        }
        else if (operand.endsWith(",X"))
        {
            mode = amode.absolute_x;
            value = compute(operand.substring(0, operand.length-2), 0);
        }
        else if (operand.endsWith(",Y"))
        {
            mode = amode.absolute_y;
            value = compute(operand.substring(0, operand.length-2), 0);
        }
        else if (operand.endsWith(",S"))
        {
            mode = amode.stack;
            value = compute(operand.substring(0, operand.length-2), 0);
        }        
        else if (operand.startsWith("<"))
        {
            mode = amode.direct;
            value = compute(operand.substring(1), 0);
        }
        else if (operand.startsWith("#"))
        {
            mode = amode.immediate;
            value = compute(operand.substring(1), 0);
        }
        else if (opcodetable[instr][amode.relative]!==null)
        {
            mode = amode.relative;
            value = compute(operand, pc);      
        }            
        else
        {
            mode = amode.absolute;
            value = compute(operand, 0);            
        }
    }

    var opcode = opcodetable[instr][mode];
    if (opcode===null) { throw "Unsupported addressing mode for '"+instr+"'"; }
    emit(opcode);
    
    if (mode===amode.absolute || mode==amode.absolute_x || mode==amode.absolute_y)
    {
        if (value<0 || value>0xFFFF) throw "Address outside 16 bit range: "+value;
        emit (value & 0xff);
        emit ((value>>8) & 0xff);
    }
    else if (mode===amode.long || mode==amode.long_x)
    {
        if (value<0 || value>0xFFFFFF) throw "Address outside 24 bit range: "+value;
        emit (value & 0xff);
        emit ((value>>8) & 0xff);
        emit ((value>>16) & 0xff);
    }
    else if (mode===amode.direct || mode==amode.stack)
    {
        if (value<0 || value>0xFF) throw "Offset outside 8 bit range: "+value;
        emit (value & 0xff);
    }
    else if (mode===amode.immediate)
    {
        if (value<0 || value>0xFF) throw "Immediate value outside 16 bit range: "+value;
        emit (value & 0xff);
        emit ((value>>8) & 0xff);
    }
    else if (mode==amode.relative && opcode===0x82)  // special treatment for long branch
    {
        var distance = value - (pc+3);
        if (distance>32767 || distance<-32768) { throw "Long branch target out of range"; }
        emit(distance & 0xff);
        emit((distance>>8) & 0xff);
    }
    else if (mode==amode.relative)
    {
        var distance = value - (pc+2);
        if (distance>127 || distance<-128) { throw "Branch target out of range"; }
        emit(distance & 0xff);
    }
}

function compute(expression, firstpassdefault)
{
    if (!finalpass && firstpassdefault!==undefined) { return firstpassdefault; }
    if (expression.startsWith("(") && expression.endsWith(").LOW.")) 
    {
        return compute(expression.substring(1,expression.length-6)) & 0xffff;
    }
    if (expression.startsWith("(") && expression.endsWith(").HIGH.")) 
    {
        return (compute(expression.substring(1,expression.length-7)) >> 16) & 0xffff;
    }
    if(/^[0-9]+$/.test(expression)) { return parseInt(expression); }
    if(/^\x24[0-9a-fA-F]+$/.test(expression)) { return parseInt(expression.substring(1),16); }
    if (identifiers.hasOwnProperty(expression)) { return identifiers[expression]; }
    throw "Unknown number or identifier '"+expression+"'";
}

function emit(code)
{
    if (finalpass) { output.push(code.toString(16).toUpperCase().padStart(2,"0")); }
    pc++;   
}


const amode = 
{
    relative:   0,
    implicit:   1,
    absolute:   2,
    absolute_x: 3,
    absolute_y: 4,
    long:       5,
    long_x:     6,
    direct:     7,
    stack:      8,
    immediate:  9
};

const opcodetable = 
{
    //     rel   impl  a     a,x   a,y   l     l,x   d     d,s   # 
    ADC: [ null, null, 0x6D, 0x7D, 0x79, 0x6F, 0x7F, 0x65, 0x63, 0x69 ],
    AND: [ null, null, 0x2D, 0x3D, 0x39, 0x2F, 0x3F, 0x25, 0x23, 0x29 ],
    ASL: [ null, 0x0A, 0x0E, 0x1E, null, null, null, 0x06, null, null ],
    BCC: [ 0x90, null, null, null, null, null, null, null, null, null ],
    BCS: [ 0xB0, null, null, null, null, null, null, null, null, null ],
    BEQ: [ 0xF0, null, null, null, null, null, null, null, null, null ],
    BIT: [ null, null, 0x2C, 0x3C, null, null, null, 0x24, null, 0x89 ],
    BMI: [ 0x30, null, null, null, null, null, null, null, null, null ],
    BNE: [ 0xD0, null, null, null, null, null, null, null, null, null ],
    BPL: [ 0x10, null, null, null, null, null, null, null, null, null ],
    BRA: [ 0x80, null, null, null, null, null, null, null, null, null ],
    BRL: [ 0x82, null, null, null, null, null, null, null, null, null ],
    BVC: [ 0x50, null, null, null, null, null, null, null, null, null ],
    BVS: [ 0x70, null, null, null, null, null, null, null, null, null ],
    CLC: [ null, 0x18, null, null, null, null, null, null, null, null ],
    CLV: [ null, 0xB8, null, null, null, null, null, null, null, null ],
    CMP: [ null, null, 0xCD, 0xDD, 0xD9, 0xCF, 0xDF, 0xC5, 0xC3, 0xC9 ],
    CPX: [ null, null, 0xEC, null, null, null, null, 0xE4, null, 0xE0 ],
    CPY: [ null, null, 0xCC, null, null, null, null, 0xC4, null, 0xC0 ],
    DEC: [ null, 0x3A, 0xCE, 0xDE, null, null, null, 0xC6, null, null ],
    DEX: [ null, 0xCA, null, null, null, null, null, null, null, null ], 
    DEY: [ null, 0x88, null, null, null, null, null, null, null, null ], 
    EOR: [ null, null, 0x4D, 0x5D, 0x59, 0x4F, 0x5F, 0x45, 0x43, 0x49 ],
    INC: [ null, 0x1A, 0xEE, 0xFE, null, null, null, 0xE6, null, null ], 
    INX: [ null, 0xE8, null, null, null, null, null, null, null, null ], 
    INY: [ null, 0xC8, null, null, null, null, null, null, null, null ], 
    JMP: [ null, null, null, null, null, 0x5C, null, null, null, null ], 
    JSL: [ null, null, null, null, null, 0x22, null, null, null, null ], 
    LDA: [ null, null, 0xAD, 0xBD, 0xB9, 0xAF, 0xBF, 0xA5, 0xA3, 0xA9 ],
    LDX: [ null, null, 0xAE, null, 0xBE, null, null, 0xA6, null, 0xA2 ],
    LDY: [ null, null, 0xAC, 0xBC, null, null, null, 0xA4, null, 0xA0 ],
    LSR: [ null, 0x4A, 0x4E, 0x5E, null, null, null, 0x46, null, null ], 
    NOP: [ null, 0xEA, null, null, null, null, null, null, null, null ], 
    ORA: [ null, null, 0x0D, 0x1D, 0x19, 0x0F, 0x1F, 0x05, 0x03, 0x09 ],
    PEA: [ null, null, null, null, null, null, null, null, null, 0xF4 ],
    PHA: [ null, 0x48, null, null, null, null, null, null, null, null ], 
    PHD: [ null, 0x0B, null, null, null, null, null, null, null, null ], 
    PHX: [ null, 0xDA, null, null, null, null, null, null, null, null ], 
    PHY: [ null, 0x5A, null, null, null, null, null, null, null, null ], 
    PLA: [ null, 0x68, null, null, null, null, null, null, null, null ], 
    PLD: [ null, 0x2B, null, null, null, null, null, null, null, null ], 
    PLX: [ null, 0xFA, null, null, null, null, null, null, null, null ], 
    PLY: [ null, 0x7A, null, null, null, null, null, null, null, null ], 
    ROL: [ null, 0x2A, 0x2E, 0x3E, null, null, null, 0x26, null, null ], 
    ROR: [ null, 0x6A, 0x6E, 0x7E, null, null, null, 0x66, null, null ], 
    RTL: [ null, 0x6B, null, null, null, null, null, null, null, null ], 
    SBC: [ null, null, 0xED, 0xFD, 0xF9, 0xEF, 0xFF, 0xE5, 0xE3, 0xE9 ],
    SEC: [ null, 0x38, null, null, null, null, null, null, null, null ], 
    STA: [ null, null, 0x8D, 0x9D, 0x99, 0x8F, 0x9F, 0x85, 0x83, null ],
    STP: [ null, 0xDB, null, null, null, null, null, null, null, null ], 
    STX: [ null, null, 0x8E, null, null, null, null, 0x86, null, null ], 
    STY: [ null, null, 0x8C, null, null, null, null, 0x84, null, null ], 
    STZ: [ null, null, 0x9C, 0x9E, null, null, null, 0x64, null, null ],
    TAX: [ null, 0xAA, null, null, null, null, null, null, null, null ], 
    TAY: [ null, 0xA8, null, null, null, null, null, null, null, null ], 
    TCD: [ null, 0x5B, null, null, null, null, null, null, null, null ], 
    TCS: [ null, 0x1B, null, null, null, null, null, null, null, null ], 
    TDC: [ null, 0x7B, null, null, null, null, null, null, null, null ], 
    TRB: [ null, null, 0x1C, null, null, null, null, 0x14, null, null ], 
    TSB: [ null, null, 0x0C, null, null, null, null, 0x04, null, null ],
    TSC: [ null, 0x3B, null, null, null, null, null, null, null, null ], 
    TSX: [ null, 0xBA, null, null, null, null, null, null, null, null ], 
    TXA: [ null, 0x8A, null, null, null, null, null, null, null, null ], 
    TXS: [ null, 0x9A, null, null, null, null, null, null, null, null ], 
    TXY: [ null, 0x9B, null, null, null, null, null, null, null, null ], 
    TYA: [ null, 0x98, null, null, null, null, null, null, null, null ], 
    TYX: [ null, 0xBB, null, null, null, null, null, null, null, null ], 
    XBA: [ null, 0xEB, null, null, null, null, null, null, null, null ]
};
