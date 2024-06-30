package br.com.luanmedeirossilveira.cooking_gas_sensor.repository;

import br.com.luanmedeirossilveira.cooking_gas_sensor.model.SensorData;
import org.springframework.data.jpa.repository.JpaRepository;

public interface SensorDataRepository extends JpaRepository<SensorData, Long> {
}