import { Component, Input, OnInit } from '@angular/core';
import { Broker, ProviderService } from '../provider.service';
import { v4 as uuidv4 } from 'uuid';

@Component({
  selector: 'app-brokers',
  templateUrl: './brokers.component.html'
})
export class BrokersComponent implements OnInit {
  @Input() current!: Broker;
  user: Broker = new Broker();
  id = 't' + uuidv4();

  constructor(private provider: ProviderService) { }
  ngOnInit(): void { this.provider.get_user().then((data) => { this.user = data; }); }
}
