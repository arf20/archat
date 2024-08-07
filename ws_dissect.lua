acp_proto = Proto("ACP","arf chat protocol")

acp_magic = ProtoField.uint32("acp.magic", "acp_magic", base.HEX)
acp_type = ProtoField.uint8("acp.type", "acp_type", base.HEX)
acp_flags = ProtoField.uint8("acp.flags", "acp_flags", base.HEX)
acp_length = ProtoField.uint16("acp.length", "acp_length", base.DEC)
acp_src_uid = ProtoField.uint32("acp.src_uid", "acp_src_uid", base.DEC)

acp_rid = ProtoField.uint16("acp.rid", "acp_rid", base.DEC)
acp_nick = ProtoField.string("acp.nick", "acp_nick", base.ASCII)
acp_host = ProtoField.string("acp.host", "acp_host", base.ASCII)
acp_rname = ProtoField.string("acp.rname", "acp_rname", base.ASCII)
acp_rmsg = ProtoField.string("acp.rmsg", "acp_rmsg", base.ASCII)

acp_proto.fields = { acp_magic, acp_type, acp_flags, acp_length, acp_src_uid,
    acp_rid, acp_nick, acp_host, acp_rname, acp_rmsg }

-- create a function to dissect it
function acp_proto.dissector(buffer,pinfo,tree)
    length = buffer:len()
    if length == 0 then return end

    pinfo.cols.protocol = "ACP"
    local subtree = tree:add(acp_proto,buffer(),"arf chat protocol data")

    -- header
    subtree:add_le(acp_magic, buffer(0,4))

    local type_num = buffer(4,1):le_uint()
    local type_name = get_type_name(type_num)
    print("type_num: " .. type_num)
    subtree:add_le(acp_type, buffer(4,1)):append_text(" (" .. type_name .. ")")

    subtree:add_le(acp_flags, buffer(5,1))
    subtree:add_le(acp_length, buffer(6,2))
    subtree:add_le(acp_src_uid, buffer(8,4))

    -- data
        if type_num == 0 then 
    elseif type_num == 1 then 
    elseif type_num == 2 then 
        subtree:add_le(acp_rid, buffer(12,2))

        local string_length = get_str_len(buffer, 16)
        subtree:add_le(acp_nick, buffer(16,string_length))

    elseif type_num == 3 then type_name = "JOIN"
    elseif type_num == 4 then type_name = "RMSG" end

end

function get_str_len(buffer, off)
    local string_length
    for i = off, length - 1, 1 do
      if (buffer(i,1):le_uint() == 0) then
        string_length = i - 20
        break
      end
    end
    return string_length
end

function get_type_name(type_num)
    local type_name = "Unknown"
  
        if type_num == 0 then type_name = "NOP"
    elseif type_num == 1 then type_name = "PING"
    elseif type_num == 2 then type_name = "PONG"
    elseif type_num == 3 then type_name = "JOIN"
    elseif type_num == 4 then type_name = "RMSG" end
  
    return type_name
end
  

-- load the udp.port table
udp_table = DissectorTable.get("udp.port")
-- register our protocol to handle udp port 42069
udp_table:add(42069,acp_proto)
