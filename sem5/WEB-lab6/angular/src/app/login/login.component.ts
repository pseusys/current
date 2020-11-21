import { Component, OnInit } from '@angular/core';
import {ProviderService} from '../provider.service';
import {Router} from '@angular/router';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {
  username = '';
  password = '';

  constructor(private router: Router, private provider: ProviderService) { }

  ngOnInit(): void { }

  on_signin(): void {
    this.provider.create_user(this.username, this.password).then((user) => {
      if (user.member()) { this.router.navigate(['/']); }
    });
  }

  on_login(): void {
    this.provider.login(this.username, this.password).then((user) => {
      if (user.member()) { this.router.navigate(['/']); }
    });
  }
}
