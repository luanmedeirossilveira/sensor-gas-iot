package br.com.luanmedeirossilveira.cooking_gas_sensor.service;

import br.com.luanmedeirossilveira.cooking_gas_sensor.model.SensorData;
import br.com.luanmedeirossilveira.cooking_gas_sensor.repository.SensorDataRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.List;

@Service
public class SensorDataService {

    private SensorDataRepository sensorDataRepository;

    public SensorDataService(SensorDataRepository sensorDataRepository) {
        this.sensorDataRepository = sensorDataRepository;
    }

    public List<SensorData> getAllReadings() {
        return sensorDataRepository.findAll();
    }

    public SensorData saveReading(float sensorValue) {
        SensorData reading = new SensorData();
        reading.setSensorValue(sensorValue);
        reading.setTimestamp(LocalDateTime.now());
        return sensorDataRepository.save(reading);
    }
}
