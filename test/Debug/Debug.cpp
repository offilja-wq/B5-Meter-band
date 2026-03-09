void Networking::handleDebug()
{
#if NETWORKING_DEBUG
    static char buf[32] = {};
    static size_t bufIndex = 0;

    while (Serial.available())
    {
        char ch = Serial.read();

        if (ch == '\n')
        {
            buf[bufIndex] = 0x00;

            switch (buf[0])
            {
            case COMMAND_RESTART:
            case COMMAND_PAIR_CONTROLLER:
            {
                // Packet packet = {
                //     .identity = this->identity,
                //     .command = (CommandType)buf[0],
                // };

                stringToMac(&buf[1], packet.data);

                if (memcmp(packet.data, this->mac, sizeof(this->mac)) == 0)
                {
                    this->monitorPacket(this->mac, &packet);

                    this->receiveInternal(this->mac, &packet);
                    this->receiveCallback(this->mac, &packet);
                }
                else
                    this->send(&packet);

                break;
            }
            case COMMAND_RESET:
            {
                static Packet packet = {
                    .identity = this->identity
                };

                this->send(&packet);
                this->receiveInternal(this->mac, &packet);

                break;
            }
            default:
                ESP_LOGW(TAG, "Unknown debug command: 0x%02X", buf[0]);
                break;
            }

            memset(buf, 0, sizeof(buf));
            bufIndex = 0;
        }
        else
        {
            buf[bufIndex++] = ch;
        }
    }
#endif
}