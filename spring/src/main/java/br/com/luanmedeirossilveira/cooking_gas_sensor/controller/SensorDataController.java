package br.com.luanmedeirossilveira.cooking_gas_sensor.controller;

import br.com.luanmedeirossilveira.cooking_gas_sensor.model.SensorData;
import br.com.luanmedeirossilveira.cooking_gas_sensor.service.SensorDataService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.List;

@RestController
@RequestMapping("/api")
public class SensorDataController {

    @Autowired
    private SensorDataService sensorDataService;

    @PostMapping("/sensor-data")
    public SensorData saveSensorData(@RequestBody SensorData sensorData) {
        return sensorDataService.saveReading(sensorData.getSensorValue());
    }

    @GetMapping("/sensor-data")
    public List<SensorData> getSensorData() {
        return sensorDataService.getAllReadings();
    }
}
