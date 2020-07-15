module AzureSphereSampleDevice
  class AzureSphereSampleDevice_5vg
    attr_reader :buttonPress
    attr_reader :temperature

	def initialize
	  @temperature = 28
      @manufacturer = "manufacturer"
      @model = "model"
      @statusLED = false
    end

    def TriggerAlarm(peyload)
      puts "execute TriggerAlarm " + peyload
      "{\"Message\":\"execute TriggerAlarm with Method\"}"
    end

    def set_manufacturer(value)
      @manufacturer = value
      puts "set manufacturer " + value.to_s
    end

    def set_model(value)
      @model = value
      puts "set model " + value.to_s
    end

    def set_status_l_e_d(value)
      @statusLED = value
      puts "set StatusLED " + value.to_s
    end

    def recv_twin(peyload)
      json = JSON.parse(peyload)
      desired = json["desired"]
      if desired == nil
        desired = json
      end
      desired.each{|key, obj|
        case key
        when "manufacturer"
          value = obj["value"]
          if value != nil
            set_manufacturer(value)
            desired[key] = {value: @manufacturer, status: "success"}
          else
            desired[key] = nil
          end
        when "model"
          value = obj["value"]
          if value != nil
            set_model(value)
            desired[key] = {value: @model, status: "success"}
          else
            desired[key] = nil
          end
        when "StatusLED"
          value = obj["value"]
          if value != nil
            set_status_l_e_d(value)
            desired[key] = {value: @statusLED, status: "success"}
          else
            desired[key] = nil
          end
        else
          desired[key] = nil
        end
      }
      desired.to_json
    end

    def get_message
      data = {
        ButtonPress: @buttonPress,
        Temperature: @temperature,
      }.to_json
      message = AzureIoT::Message.new(data)
      return message
    end

    def measure
	    @buttonPress = false
	    delta = (rand() % 41) / 20.0 - 1.0
      @temperature += delta
    end
  end
end

twin = AzureSphereSampleDevice::AzureSphereSampleDevice_5vg.new

client = AzureIoT::DeviceClient.new()
client.set_twin(twin)

while true do
  twin.measure
  message = twin.get_message

  done = false
  client.send_event(message) do
    puts "sent message"
    done = true
  end

  count = 5000
  while !done do
    client.do_work
    sleep(0.001)
    if count > 0 then
      count -= 1
    end
  end

  if count > 0 then
    sleep(0.001 * count)
  end
end
