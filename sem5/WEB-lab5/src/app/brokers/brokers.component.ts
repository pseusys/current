import {Component, Input, OnInit} from '@angular/core';
import {Broker} from '../provider.service';

@Component({
  selector: 'app-brokers',
  templateUrl: './brokers.component.html'
})
export class BrokersComponent implements OnInit {
  @Input() current!: Broker;

  constructor() { }
  ngOnInit(): void { }
}
